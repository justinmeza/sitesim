#ifndef EDGE_HPP
#define EDGE_HPP

#include "Region.hpp"

#define PI 3.14159265

using namespace std;

class Edge {
	public:
		string name;
		map<string, float> mappings;
		function<int(float)> demand;

		Edge(string name, auto demand) {
			this->name = name;
			this->demand = demand;
			cout << "Creating new Edge: " << this->name << endl;
		}

		void addMapping(string r, float p) {
			this->mappings.insert(pair(r, p));
			cout << "Adding mapping: " << r << endl;
			cout << "  at ratio: " << p << endl;
		}

		int getRPS(string r, int s) {
			return mappings.at(r) * this->demand((float)(s % (60 * 60 * 24)) / 60 / 60 / 24);
		}
};

#endif  // EDGE_HPP
