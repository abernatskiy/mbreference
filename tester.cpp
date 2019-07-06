#include <iostream>
#include <vector>
#include <fstream>

#include "constants.h"
#include "utils/strutil.h"
//#include "individualALRuler.h"
#include "evolution/evolverMAFPO.h"

using namespace std;

int main(int argc, char** argv) {

	if( argc!=1 ) {
		cout << "Usage: just pipe genomes to stdin and read evaluations from stdout" << endl;
		return 1;
	}

	// Getting the description string from the standard input
	string indivDesc;
	getline(cin, indivDesc);
//	string indivDesc = "debug genome goes here";

	TestIndividual testIndiv;

	testIndiv.loadGenomeStr(indivDesc);

	testIndiv.evaluate();
	cout << evaluationsToCompactString(testIndiv.evals) << endl;

	return 0;
}
