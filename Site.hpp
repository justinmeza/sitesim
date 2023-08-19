#ifndef SITE_HPP
#define SITE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Region.hpp"

using namespace std;

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

#endif
