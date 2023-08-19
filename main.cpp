#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>

#include "Service.hpp"
#include "Stack.hpp"
#include "Region.hpp"
#include "Edge.hpp"
#include "Site.hpp"

using namespace std;

Site *site;

int main(int argc, char** argv) {
	cout << "Initializing SiteSim..." << endl;

	Region *west = new Region("West");
	{
		Service *frontend = new RegionalService("Frontend");
		Service *rcache = new RegionalService("RCache");
		LoadBalancedService *lbcache = new LoadBalancedService("LBCache");
		lbcache->addPolicy("West", 0.3);
		lbcache->addPolicy("East", 0.7);
		LocalityService *lcache = new LocalityService("LCache");
		lcache->addRegion("West");
		lcache->addRegion("East");
		Service *database = new RegionalService("Database");

		Service *cache = lbcache;
		frontend->addDependency(cache);
		cache->addDependency(database, 0.1);

		Stack *app = new Stack("App", frontend);

		west->addStack(app);
	}

	Region *east = new Region("East");
	{
		Service *frontend = new RegionalService("Frontend");
		Service *rcache = new RegionalService("RCache");
		LoadBalancedService *lbcache = new LoadBalancedService("LBCache");
		lbcache->addPolicy("West", 0.3);
		lbcache->addPolicy("East", 0.7);
		LocalityService *lcache = new LocalityService("LCache");
		lcache->addRegion("West");
		lcache->addRegion("East");
		Service *database = new RegionalService("Database");

		Service *cache = lbcache;
		frontend->addDependency(cache);
		cache->addDependency(database, 0.1);

		Stack *app = new Stack("App", frontend);

		east->addStack(app);
	}

	Edge *north = new Edge("North", [](auto p) { return 50 + sin(p * 2 * PI) * 50; });
	north->addMapping("West", 0.5);
	north->addMapping("East", 0.5);

	Edge *south = new Edge("South", [](auto p) { return 50 + cos(p * 2 * PI) * 50; });
	south->addMapping("West", 0.25);
	south->addMapping("East", 0.75);

	site = new Site("Site");
	site->addEdge(north);
	site->addEdge(south);
	site->addRegion(west);
	site->addRegion(east);

	auto steps = 60 * 60 * 12;
	for (int s = 0; s < steps; s++) {
		cout << "North RPS to West: " << north->getRPS("West", s) << endl;
		cout << "South RPS to West: " << south->getRPS("West", s) << endl;
		site->doStep(s);
	}

	cout << "Done with simulation!" << endl;

	site->printState();

	cout << "Finalizing SiteSim..." << endl;
	cout << "Done!" << endl;
}
