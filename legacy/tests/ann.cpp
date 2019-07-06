#include <iostream>

#include "../ann.h"

using namespace std;

void printVec(vector<double> v) {
	for(auto f: v)
		cout << f << ' ';
};

int main() {
	const vector<unsigned> lsizes = {10,10,10};

	ArtificialNeuralNetwork a(lsizes);
	for(unsigned l=0; l<2; l++)
		for(unsigned i=0; i<10; i++) {
//			cout << 'l' << l << ' ' << 'i' << i << ' ' << 'j' << j << endl;
			a.weights[l][i][9-i] = (i+l)%2==0 ? 1.0 : -1.0;
			a.weights[l][10][i] = 0.1;
		}
	a.weights[0][0][0] = 1.2;

	cout << "Created a test ANN. Weights:" << endl;
	a.drawWeights();

	vector<double> in = {0,1,2,3,4,5,6,7,8,9};
	auto out = a.forward(in);

	string compactA = a.compactStr();
	cout << "Compact representation of the ANN: " << compactA << endl << endl;

	cout << "Pushed vector ";
	printVec(in);
	cout << " through the ANN. Result:" << endl;
	printVec(out);
	cout << endl << endl;

	cout << "Connections counts: input layer " << a.countConnections(0) << ", output layer " << a.countConnections(1) << endl;
	cout << "Discnnections counts: input layer " << a.countDisconnections(0) << ", output layer " << a.countDisconnections(1) << endl;

	cout << endl;

	cout << "All connections of layer 0:";
	for(auto ip : a.allConnectionsIndices(0)) {
		unsigned from, to;
		tie(from, to) = ip;
		cout << " (" << from << "," << to << "),";
	}
	cout << endl << endl;

	cout << "All disconnections of layer 1:";
	for(auto ip : a.allDisconnectionsIndices(1)) {
		unsigned from, to;
		tie(from, to) = ip;
		cout << " (" << from << "," << to << "),";
	}
	cout << endl << endl;

	cout << "Resetting the ANN and drawing weights:" << endl;
	a.reset();
	a.drawWeights();

	cout << "Restoring the ANN from compact string. Result:" << endl;

	ArtificialNeuralNetwork aprime(lsizes);
	aprime.loadCompactStr(compactA);
	aprime.drawWeights();

	string compactAPrime = aprime.compactStr();
	cout << "Compact strings of the original and restored ANN are " << (compactAPrime==compactA?"identical":"different") << endl;

	return 0;
}
