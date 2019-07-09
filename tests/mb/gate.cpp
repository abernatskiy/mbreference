#include <iostream>

#include "../../mb/gate.h"

using namespace std;

void printBrainStates(bool* states, unsigned numStates) {
	cout << static_cast<unsigned>(states[0]);
	for(unsigned i=1; i<numStates; i++)
		cout << static_cast<unsigned>(states[i]);
}

int main() {

	/********** Testing operation of a manually constructed gate **********/

	const unsigned numStates = 10;
	bool mbStates[numStates];
	bool newMbStates[numStates];
	fill(mbStates, mbStates+numStates, false);
	fill(newMbStates, newMbStates+numStates, false);

	Gate g;
	g.id = 8;
	g.inputShifts.push_back(0);
	g.inputShifts.push_back(1);
	g.outputShifts.push_back(1);
	g.outputShifts.push_back(2);
	g.table.push_back({true, false});
	g.table.push_back({false, true});
	g.table.push_back({true, false});
	g.table.push_back({false, false});

	const unsigned numTimeSteps = 5;
	cout << "Initial brain state: ";
	printBrainStates(mbStates, numStates);
	cout << endl;
	for(unsigned t=0; t<numTimeSteps; t++) {
		g.update(mbStates, newMbStates);
		copy(newMbStates, newMbStates+numStates, mbStates);
		fill(newMbStates, newMbStates+numStates, false);
		cout << "After update " << t << ": ";
		printBrainStates(mbStates, numStates);
		cout << endl;
	}

	/********** Testing serialization and deserialization to JSON **********/

	json11::Json gateJSON0 = g.to_json();
	string gdump0 = gateJSON0.dump();
	cout << "Gate JSON 0: " << gdump0 << endl;

	string err;
	json11::Json jsonFromDump0 = json11::Json::parse(gdump0, err);
	cout << "Gate JSON 0 parsed. Error message was \"" << err << "\"" << endl;

	Gate g1;
	g1.from_json(jsonFromDump0);

	json11::Json gateJSON1 = g1.to_json();
	string gdump1 = gateJSON1.dump();
	cout << "Gate JSON 1: " << gdump1 << endl;

	/********** Testing randomization and mutations **********/

	mt19937_64 rng(4223334);
	const UIntRange inputNumRange {1, 4};
	const UIntRange inputStatesRange {0, numStates-1};
	const UIntRange outputNumRange {1, 4};
	const UIntRange outputStatesRange {0, numStates/2-1}; // imagining that the last half of states are actually brain inputs

	Gate g2;
	g2.id = 2;
	g2.randomize(inputNumRange, inputStatesRange, outputNumRange, outputStatesRange, rng);
	json11::Json gateJSON2 = g2.to_json();
	string gdump2 = gateJSON2.dump();
	cout << "Gate JSON 2: " << gdump2 << endl;

	Gate g3;
	g3.id = 3;
	g3.randomize(inputNumRange, inputStatesRange, outputNumRange, outputStatesRange, rng);
	json11::Json gateJSON3 = g3.to_json();
	string gdump3 = gateJSON3.dump();
	cout << "Gate JSON 3: " << gdump3 << endl;

	cout << "Mutating gate 3" << endl;

	g3.rewireAConnectionRandomly(inputStatesRange, outputStatesRange, rng);
	gateJSON3 = g3.to_json();
	gdump3 = gateJSON3.dump();

	cout << "After rewiring a connection: " << gdump3 << endl;

	g3.modifyTableRandomly(rng);
	gateJSON3 = g3.to_json();
	gdump3 = gateJSON3.dump();

	cout << "After a table modification: " << gdump3 << endl;

	return 0;
};
