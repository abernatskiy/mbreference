#include <iostream>

#include "../../mb/gate.h"

using namespace std;

void printBrainStates(bool* states, unsigned numStates) {
	cout << static_cast<unsigned>(states[0]);
	for(unsigned i=1; i<numStates; i++)
		cout << static_cast<unsigned>(states[i]);
}

int main() {

	const unsigned numStates = 10;
	bool mbStates[numStates];
	for(unsigned i=0; i<numStates; i++)
		mbStates[i] = false;

	Gate g;
	g.inputs.push_back(mbStates);
	g.inputs.push_back(mbStates+1);
	g.outputs.push_back(mbStates+1);
	g.outputs.push_back(mbStates+2);
	g.table.push_back({true, false});
	g.table.push_back({false, true});
	g.table.push_back({true, false});
	g.table.push_back({false, false});

	const unsigned numTimeSteps = 5;
	cout << "Initial brain state: ";
	printBrainStates(mbStates, numStates);
	cout << endl;
	for(unsigned t=0; t<numTimeSteps; t++) {
		g.update();
		cout << "After update " << t << ": ";
		printBrainStates(mbStates, numStates);
		cout << endl;
	}

	return 0;
};
