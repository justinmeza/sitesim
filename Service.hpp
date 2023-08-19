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
		vector<Service *> dependencies;
		int requests = 0;

		Service(string name);
		void addDependency(Service *s);
		Service *getService(string name);
		virtual void doStep(int rps) {}
		void printState();
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

#endif  // SERVICE_HPP