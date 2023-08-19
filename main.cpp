#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <iomanip>

using namespace std;

class Service {
	public:
		string name;
		vector<Service *> dependencies;
		int requests = 0;

		Service(string name) {
			this->name = name;
			cout << "Creating new Service: " << this->name << endl;
		}

		void addDependency(Service *s) {
			this->dependencies.push_back(s);
			cout << "Adding dependence: " << this->name << " -> " << s->name << endl;
		}

		Service *getService(string name) {
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

		virtual void doStep(int rps) {}

		void printState() {
			cout << "State for Service: " << this->name << endl;
			cout << "  requests: " << this->requests << endl;
			for (auto s = dependencies.begin(); s != dependencies.end(); s++) {
				(*s)->printState();
			}
		}

	private:
};

class RegionalService : public Service {
	public:
		RegionalService(string name) : Service(name) {}
		map<string, float> policies;

		void doStep(int rps) {
			cout << "Doing step for RegionalService: " << this->name << endl;
			this->requests += rps;
			cout << "  Serving " << rps << " requests" << endl;
			cout << "  Total requests: " << this->requests << endl;
			cout << "  Sending requests to dependencies" << endl;
			for (auto s = dependencies.begin(); s != dependencies.end(); s++) {
				(*s)->doStep(rps);
			}
		}
};

class Stack {
	public:
		string name;
		Service *source;  // TODO:  Make this a vector.
		int rps;

		Stack(string name, Service *source, int rps) : source(source) {
			this->name = name;
			this->rps = rps;
			cout << "Creating new Stack: " << this->name << endl;
			cout << "  with source: " << this->source->name << endl;
			cout << "  and RPS: " << this->rps << endl;
		}

		Service *getService(string name) {
			return source->getService(name);
		}

		void doStep() {
			cout << "Doing step for Stack: " << this->name << endl;
			source->doStep(this->rps);
		}

		void printState() {
			cout << "State for Stack: " << this->name << endl;
			source->printState();
		}

	private:
};

class Region {
	public:
		string name;
		vector<Stack *> stacks;

		Region(string name) {
			this->name = name;
			cout << "Creating new Region: " << this->name << endl;
		}

		void addStack(Stack *s) {
			this->stacks.push_back(s);
			cout << "Adding Stack: " << s->name << endl;
		}

		Service* getService(string name) {
			for (auto s = stacks.begin(); s != stacks.end(); s++) {
				Service *search = (*s)->getService(name);
				if (search->name == name) {
					return search;
				}
			}

			return nullptr;
		}

		void doStep() {
			cout << "Doing step for Region: " << this->name << endl;
			for (auto s = stacks.begin(); s != stacks.end(); s++) {
				(*s)->doStep();
			}
		}

		void printState() {
			cout << "State for Region: " << this->name << endl;
			for (auto s = stacks.begin(); s != stacks.end(); s++) {
				(*s)->printState();
			}
		}

	private:
};

class Site {
	public:
		string name;
		vector<Region *> regions;

		Site(string name) {
			this->name = name;
			cout << "Creating new Site: " << this->name << endl;
		}

		void addRegion(Region *r) {
			this->regions.push_back(r);
			cout << "Adding Region: " << r->name << endl;
		}

		Region *getRegion(string name) {
			for (auto r = regions.begin(); r != regions.end(); r++) {
				if ((*r)->name == name) {
					return *r;
				}
			}
			return nullptr;
		}

		void doStep() {
			cout << "Doing step for Site: " << this->name << endl;
			for (auto r = regions.begin(); r != regions.end(); r++) {
				(*r)->doStep();
			}
		}

		void printState() {
			cout << "State for Site: " << this->name << endl;
			for (auto r = regions.begin(); r != regions.end(); r++) {
				(*r)->printState();
			}
		}

	private:
};

Site *site;

class LoadBalancedService : public Service {
	public:
		LoadBalancedService(string name) : Service(name) {}
		map<string, float> policies;

		void doStep(int rps) {
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

		void addPolicy(string r, float p) {
			this->policies.insert(pair<string, float>(r, p));
		}
	private:
};

int main(int argc, char** argv) {
	cout << "Initializing SiteSim..." << endl;

	Region *west = new Region("West");
	{
		Service *frontend = new RegionalService("Frontend");
		Service *cache = new RegionalService("Cache");
		LoadBalancedService *lbcache = new LoadBalancedService("LBCache");
		lbcache->addPolicy("West", 0.3);
		lbcache->addPolicy("East", 0.7);
		Service *database = new RegionalService("Database");

		//frontend->addDependency(cache);
		//cache->addDependency(database);
		frontend->addDependency(lbcache);
		lbcache->addDependency(database);

		Stack *app = new Stack("App", frontend, 100);

		west->addStack(app);
	}

	Region *east = new Region("East");
	{
		Service *frontend = new RegionalService("Frontend");
		Service *cache = new RegionalService("Cache");
		LoadBalancedService *lbcache = new LoadBalancedService("LBCache");
		lbcache->addPolicy("West", 0.3);
		lbcache->addPolicy("East", 0.7);
		Service *database = new RegionalService("Database");

		//frontend->addDependency(cache);
		//cache->addDependency(database);
		frontend->addDependency(lbcache);
		lbcache->addDependency(database);

		Stack *app = new Stack("App", frontend, 100);

		east->addStack(app);
	}

	site = new Site("Site");
	site->addRegion(west);
	site->addRegion(east);

	auto steps = 100;
	for (int s = 0; s < steps; s++) {
		site->doStep();
	}

	cout << "Done with simulation!" << endl;

	site->printState();

	cout << "Finalizing SiteSim..." << endl;
	cout << "Done!" << endl;
}
