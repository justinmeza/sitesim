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

void Service::addDependency(Service *s) {
	this->dependencies.push_back(s);
	cout << "Adding dependence: " << this->name << " -> " << s->name << endl;
}

Service *Service::getService(string name) {
	if (this->name == name) {
		return this;
	}

	for (auto s = dependencies.begin(); s != dependencies.end(); s++) {
		Service *search = (*s)->getService(name);
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
		(*s)->printState();
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
	cout << "  Serving " << rps << " requests" << endl;
	cout << "  Total requests: " << this->requests << endl;
	cout << "  Sending requests to dependencies" << endl;
	for (auto s = dependencies.begin(); s != dependencies.end(); s++) {
		(*s)->doStep(rps);
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//  LoadBalancedService
//
///////////////////////////////////////////////////////////////////////////////

extern Site *site;

void LoadBalancedService::doStep(int rps) {
	cout << "Doing step for LoadBalancedService: " << this->name << endl;
	this->requests += rps;
	cout << "  Serving " << rps << " requests" << endl;
	cout << "  Total requests: " << this->requests << endl;
	cout << "  Sending requests to dependencies" << endl;

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
			cout << "  in Region: " << r->name << endl;
			cout << "  with probability: " << probability << endl;
			cout << "  Serving " << rps << " requests" << endl;
			cout << "  Total requests: " << this->requests << endl;
			cout << "  Sending requests to dependencies" << endl;
			for (auto s = dest->dependencies.begin(); s != dest->dependencies.end(); s++) {
				(*s)->doStep(rps);
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
	this->policies.insert(pair<string, float>(r, p));
}
