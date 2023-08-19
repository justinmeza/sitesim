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

#endif  // STACK_HPP
