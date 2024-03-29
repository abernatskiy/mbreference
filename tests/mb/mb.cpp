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

	const unsigned numSensors = 1;
	const UIntRange inputsRange {0, numStates-1};
	const UIntRange outputsRange {numSensors, numStates-1};

	MarkovBrain mb1;
	mt19937_64 rng(324324);
	mb1.makeRandom(inputsRange, outputsRange, 5, rng);

	cout << mb1.to_json_str() << endl;

	fill(states, states+numStates, false);
	states[0] = true;
	printBrainStates(states, numStates);
	cout << endl;
	for(unsigned t=0; t<5; t++) {
		fill(newStates, newStates+numStates, false);
		mb1.update(states, newStates);
		copy(newStates, newStates+numStates, states);
		printBrainStates(states, numStates);
		cout << endl;
	}

	/********** Mutation **********/

	cout << "Mutating the last brain" << endl;
	mb1.mutate(inputsRange, outputsRange, rng);
	cout << mb1.to_json_str() << endl;

	fill(states, states+numStates, false);
	states[0] = true;
	printBrainStates(states, numStates);
	cout << endl;
	for(unsigned t=0; t<5; t++) {
		fill(newStates, newStates+numStates, false);
		mb1.update(states, newStates);
		copy(newStates, newStates+numStates, states);
		printBrainStates(states, numStates);
		cout << endl;
	}

	cout << "Mutating the last brain" << endl;
	mb1.mutate(inputsRange, outputsRange, rng);
	cout << mb1.to_json_str() << endl;

	fill(states, states+numStates, false);
	states[0] = true;
	printBrainStates(states, numStates);
	cout << endl;
	for(unsigned t=0; t<5; t++) {
		fill(newStates, newStates+numStates, false);
		mb1.update(states, newStates);
		copy(newStates, newStates+numStates, states);
		printBrainStates(states, numStates);
		cout << endl;
	}

	cout << "Mutating the last brain" << endl;
	mb1.mutate(inputsRange, outputsRange, rng);
	cout << mb1.to_json_str() << endl;

	fill(states, states+numStates, false);
	states[0] = true;
	printBrainStates(states, numStates);
	cout << endl;
	for(unsigned t=0; t<5; t++) {
		fill(newStates, newStates+numStates, false);
		mb1.update(states, newStates);
		copy(newStates, newStates+numStates, states);
		printBrainStates(states, numStates);
		cout << endl;
	}

	cout << "Mutating the last brain" << endl;
	mb1.mutate(inputsRange, outputsRange, rng);
	cout << mb1.to_json_str() << endl;

	fill(states, states+numStates, false);
	states[0] = true;
	printBrainStates(states, numStates);
	cout << endl;
	for(unsigned t=0; t<5; t++) {
		fill(newStates, newStates+numStates, false);
		mb1.update(states, newStates);
		copy(newStates, newStates+numStates, states);
		printBrainStates(states, numStates);
		cout << endl;
	}

	/********** Metadata handling **********/

	mb1.saveMABEMetadata(23,32,45);
	cout << "The last brain after adding metadata:" << endl << mb1.to_json_str() << endl;
	cout << "Validation with the right numbers: " << (mb1.validateMABEMetadata(23,32,45) ? "OK" : "FAIL") << endl;
	cout << "Validation with the wrong numbers: " << (mb1.validateMABEMetadata(2323,32,45) ? "OK" : "FAIL") << endl;

	return 0;
}
