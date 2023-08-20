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
		int capacity;

		Service(string name, int capacity);
		void addDependency(Service *s, float factor = 1.0);
		Service *getService(string name);
		virtual void doStep(int rps) {}
		void printState();
		int getRPS(int rps, auto dependency);
		void updateStats(int rps);

	private:
		int rps_max = 0;
		float util_max = 0.0;
};

class RegionalService : public Service {
	public:
		RegionalService(string name, int capacity) : Service(name, capacity) {}
		map<string, float> policies;

		void doStep(int rps);
};

class BatchProcessingService : public Service {
	public:
		BatchProcessingService(string name, int capacity) : Service(name, capacity) {}
		map<string, float> policies;

		void doStep(int rps);
		void addPolicy(string r, float p);
		void sendRPS(int rps);
		int recieveRPS();

	private:
		int rps_acc = 0;
};

class LoadBalancedService : public Service {
	public:
		LoadBalancedService(string name, int capacity) : Service(name, capacity) {}
		map<string, float> policies;

		void doStep(int rps);
		void addPolicy(string r, float p);
		void sendRPS(int rps);
		int recieveRPS();

	private:
		int rps_acc = 0;
};

class LocalityService : public Service {
	public:
		LocalityService(string name, int capacity) : Service(name, capacity) {}
		vector<string> regions;

		void doStep(int rps);
		void addRegion(string r);
};

#endif  // SERVICE_HPP
