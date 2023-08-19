#ifndef REGION_HPP
#define REGION_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Stack.hpp"

using namespace std;

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

#endif  // REGION_HPP
