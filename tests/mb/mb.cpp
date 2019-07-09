#include <iostream>

#include "../../mb/mb.h"

using namespace std;

void printBrainStates(bool* states, unsigned numStates) {
	cout << static_cast<unsigned>(states[0]);
	for(unsigned i=1; i<numStates; i++)
		cout << static_cast<unsigned>(states[i]);
}

int main() {

	/********** Deserialization and serialization **********/

	MarkovBrain mb;
	string brainJsonStr =
		"{\"gates\":["
		"{\"id\":30,\"inputs\":[0],\"outputs\":[1],\"table\":[[0],[1]]}," // shifter by one to the left
		"{\"id\":31,\"inputs\":[0,1],\"outputs\":[2],\"table\":[[0],[1],[0],[0]]}]," // pattern detector that writes further left
		"\"numHidden\":42,"
		"\"numInputs\":42,"
		"\"numOutputs\":42}";
	mb.from_json_str(brainJsonStr, true);
	cout << mb.to_json_str() << endl;

	/********** Updating **********/

	const unsigned numStates = 10;
	bool states[numStates];
	bool newStates[numStates];

	fill(states, states+numStates, false);
	states[0] = true;
	printBrainStates(states, numStates);
	cout << endl;
	for(unsigned t=0; t<5; t++) {
		fill(newStates, newStates+numStates, false);
		mb.update(states, newStates);
		copy(newStates, newStates+numStates, states);
		printBrainStates(states, numStates);
		cout << endl;
	}

	/********** Randomization **********/

//	unsigned numSensors = 1;
//	const UIntRange inputsRange {0, numStates-1};
//	const UIntRange outputsRange {1, numStates-1};


	return 0;
}
