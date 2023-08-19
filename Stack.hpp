#ifndef STACK_HPP
#define STACK_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>

#include "Service.hpp"

using namespace std;

class Stack {
	public:
		string name;
		Service *source;  // TODO:  Make this a vector.

		Stack(string name, Service *source) : source(source) {
			this->name = name;
			cout << "Creating new Stack: " << this->name << endl;
			cout << "  with source: " << this->source->name << endl;
		}

		Service *getService(string name) {
			return source->getService(name);
		}

		void doStep(int rps) {
			cout << "Doing step for Stack: " << this->name << endl;
			source->doStep(rps);
		}

		void printState() {
			cout << "State for Stack: " << this->name << endl;
			source->printState();
		}

	private:
};

#endif  // STACK_HPP
