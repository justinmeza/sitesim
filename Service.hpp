#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class Region;

class Service {
	public:
		string name;
		vector<pair<Service *, float> > dependencies;
		int requests = 0;

		Service(string name);
		void addDependency(Service *s, float factor = 1.0);
		Service *getService(string name);
		virtual void doStep(int rps) {}
		void printState();
		int getRPS(int rps, auto dependency);
};

class RegionalService : public Service {
	public:
		RegionalService(string name) : Service(name) {}
		map<string, float> policies;

		void doStep(int rps);
};

class LoadBalancedService : public Service {
	public:
		LoadBalancedService(string name) : Service(name) {}
		map<string, float> policies;

		void doStep(int rps);
		void addPolicy(string r, float p);
};

class LocalityService : public Service {
	public:
		LocalityService(string name) : Service(name) {}
		vector<string> regions;

		void doStep(int rps);
		void addRegion(string r);
};

#endif  // SERVICE_HPP
