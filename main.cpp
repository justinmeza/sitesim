#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>

#include "Service.hpp"
#include "Stack.hpp"
#include "Region.hpp"
#include "Site.hpp"

using namespace std;

Site *site;

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
