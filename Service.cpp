#include "Service.hpp"
#include "Site.hpp"

///////////////////////////////////////////////////////////////////////////////
//
//  Service
//
///////////////////////////////////////////////////////////////////////////////

Service::Service(string name) {
	this->name = name;
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
	cout << "  requests: " << this->requests << endl;
	for (auto s = dependencies.begin(); s != dependencies.end(); s++) {
		s->first->printState();
	}
}

int Service::getRPS(int rps, auto dependency) {
	return rps * dependency->second;
}

///////////////////////////////////////////////////////////////////////////////
//
//  RegionalService
//
///////////////////////////////////////////////////////////////////////////////

void RegionalService::doStep(int rps) {
	cout << "Doing step for RegionalService: " << this->name << endl;
	this->requests += rps;
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
//  LoadBalancedService
//
///////////////////////////////////////////////////////////////////////////////

void LoadBalancedService::doStep(int rps) {
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
			Service *dest = r->getService(this->name);
			if (dest == nullptr) {
				cout << "Unable to find service: " << this->name << endl;
				cout << "Aborting!" << endl;
				exit(1);
			}
			selected = true;

			cout << "Doing step for LoadBalancedService: " << this->name << endl;
			dest->requests += rps;
			cout << "  in Region: " << r->name << endl;
			cout << "  with probability: " << probability << endl;
			cout << "  Serving " << rps << " requests" << endl;
			cout << "  Total requests: " << dest->requests << endl;
			cout << "  Sending requests to dependencies" << endl;
			for (auto s = dest->dependencies.begin(); s != dest->dependencies.end(); s++) {
				s->first->doStep(this->getRPS(rps, s));
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

void LoadBalancedService::addPolicy(string r, float p) {
	this->policies.insert(pair(r, p));
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
