#include "Service.hpp"
#include "Site.hpp"

///////////////////////////////////////////////////////////////////////////////
//
//  Service
//
///////////////////////////////////////////////////////////////////////////////

Service::Service(string name, int capacity) {
	this->name = name;
	this->capacity = capacity;
	cout << "Creating new Service: " << this->name << endl;
}

void Service::addDependency(Service *s, float factor) {
	this->dependencies.push_back(pair(s, factor));
	cout << "Adding dependence: " << this->name << " -> " << s->name << endl;
}

Service *Service::getService(string name) {
	if (this->name == name) {
		return this;
	}

	for (auto s = dependencies.begin(); s != dependencies.end(); s++) {
		Service *search = (*s).first->getService(name);
		if (search->name == name) {
			return search;
		}
	}

	return nullptr;
}

void Service::printState() {
	cout << "State for Service: " << this->name << endl;
	cout << "  requests:         " << this->requests << endl;
	cout << "  max RPS:          " << this->rps_max << endl;
	cout << "  max utilization:  " << this->util_max * 100.0 << "%" << endl;
	for (auto s = dependencies.begin(); s != dependencies.end(); s++) {
		s->first->printState();
	}
}

int Service::getRPS(int rps, auto dependency) {
	return rps * dependency->second;
}

void Service::updateStats(int rps) {
	if (rps > this->rps_max) {
		this->rps_max = rps;
	}
	float util = (float)rps / (float)capacity;
	if (util > this->util_max) {
		this->util_max = util;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//  RegionalService
//
///////////////////////////////////////////////////////////////////////////////

void RegionalService::doStep(int rps) {
	cout << "Doing step for RegionalService: " << this->name << endl;
	this->requests += rps;
	updateStats(rps);
	cout << "  Serving " << rps << " requests" << endl;
	cout << "  Total requests: " << this->requests << endl;
	cout << "  Sending requests to dependencies" << endl;
	for (auto s = dependencies.begin(); s != dependencies.end(); s++) {
		s->first->doStep(this->getRPS(rps, s));
	}
}

extern Site *site;

///////////////////////////////////////////////////////////////////////////////
//
//  BatchProcessingService
//
///////////////////////////////////////////////////////////////////////////////

void BatchProcessingService::doStep(int rps) {
	const int FLOAT_MIN = 0;
	const int FLOAT_MAX = 1;

	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);

	float pick = distr(eng);

	bool selected = false;
	for (auto p = policies.begin(); p != policies.end(); p++) {
		string region = p->first;
		float probability = p->second;

		if (pick < probability) {
			// Route traffic to service in this region.
			Region *r = site->getRegion(region);
			BatchProcessingService *dest = (BatchProcessingService *)r->getService(this->name);
			if (dest == nullptr) {
				cout << "Unable to find service: " << this->name << endl;
				cout << "Aborting!" << endl;
				exit(1);
			}
			selected = true;

			cout << "Doing step for BatchProcessingService: " << this->name << endl;
			dest->sendRPS(rps);
			cout << "  sending " << rps << " requests" << endl;
			cout << "  to Region: " << r->name << endl;
			cout << "  with probability: " << probability << endl;

			// Handle this region's RPS.
			int rps_total = recieveRPS();
			cout << "  recieving " << rps_total << " requests" << endl;
			this->requests += rps_total;
			this->updateStats(rps_total);
			cout << "  Serving " << rps_total << " requests" << endl;
			cout << "  Total requests: " << this->requests << endl;
			cout << "  Sending requests to dependencies" << endl;
			for (auto s = dest->dependencies.begin(); s != dest->dependencies.end(); s++) {
				s->first->doStep(this->getRPS(rps_total, s));
			}
			break;
		} else {
			pick -= probability;
		}
	}

	if (selected == false) {
		cout << "Unable to select Region for: " << this->name << endl;
		cout << "Aborting!" << endl;
		exit(1);
	}
}

void BatchProcessingService::addPolicy(string r, float p) {
	this->policies.insert(pair(r, p));
}

void BatchProcessingService::sendRPS(int r) {
	this->rps_acc += r;
}

int BatchProcessingService::recieveRPS() {
	int rps = this->rps_acc;
	this->rps_acc = 0;
	return rps;
}

///////////////////////////////////////////////////////////////////////////////
//
//  LoadBalancedService
//
///////////////////////////////////////////////////////////////////////////////

void LoadBalancedService::doStep(int rps) {
	// Send proportion of traffic to different regions.
	for (auto p = policies.begin(); p != policies.end(); p++) {
		string region = p->first;
		float proportion = p->second;

		Region *r = site->getRegion(region);
		LoadBalancedService *dest = (LoadBalancedService *)r->getService(this->name);
		if (dest == nullptr) {
			cout << "Unable to find service: " << this->name << endl;
			cout << "Aborting!" << endl;
			exit(1);
		}

		cout << "Doing step for LoadBalancedService: " << this->name << endl;
		int rps_prop = proportion * rps;
		dest->sendRPS(rps_prop);
		cout << "  sending " << rps_prop << " requests" << endl;
		cout << "  to Region: " << r->name << endl;
		cout << "  with proportion: " << proportion << endl;
	}

	// Handle this region's RPS.
	int rps_total = recieveRPS();
	cout << "  recieving " << rps_total << " requests" << endl;
	this->requests += rps_total;
	this->updateStats(rps_total);
	cout << "  Serving " << rps_total << " requests" << endl;
	cout << "  Total requests: " << this->requests << endl;
	cout << "  Sending requests to dependencies" << endl;
	for (auto s = dependencies.begin(); s != dependencies.end(); s++) {
		s->first->doStep(this->getRPS(rps_total, s));
	}
}

void LoadBalancedService::addPolicy(string r, float p) {
	this->policies.insert(pair(r, p));
}

void LoadBalancedService::sendRPS(int r) {
	this->rps_acc += r;
}

int LoadBalancedService::recieveRPS() {
	int rps = this->rps_acc;
	this->rps_acc = 0;
	return rps;
}

///////////////////////////////////////////////////////////////////////////////
//
//  LocalityService
//
///////////////////////////////////////////////////////////////////////////////

void LocalityService::doStep(int rps) {
	const int FLOAT_MIN = 0;
	const int FLOAT_MAX = 1;

	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);

	float pick = distr(eng);

	unsigned int selection = site->regions.size() * pick;
	Region *r = site->regions[selection];
	Service *dest = r->getService(this->name);

	cout << "Doing step for LocalityService: " << this->name << endl;
	dest->requests += rps;
	updateStats(rps);
	cout << "  in Region: " << r->name << endl;
	cout << "  with pick: " << pick << endl;
	cout << "  Serving " << rps << " requests" << endl;
	cout << "  Total requests: " << dest->requests << endl;
	cout << "  Sending requests to dependencies" << endl;
	for (auto s = dest->dependencies.begin(); s != dest->dependencies.end(); s++) {
		s->first->doStep(this->getRPS(rps, s));
	}
}

void LocalityService::addRegion(string r) {
	this->regions.push_back(r);
}
