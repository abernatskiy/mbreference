#include <iostream>
#include <vector>
#include <fstream>

#include "constants.h"
#include "utils/strutil.h"
//#include "individualALRuler.h"
#include "evolution/evolverMAFPO.h"

using namespace std;

int main(int argc, char** argv) {

	if( !(argc==3 || argc==4) || (!isLegalUInt(argv[1])) || (!isLegalUInt(argv[2])) ) {
		cout << "Usage: " << argv[0] << " <evolution_random_seed> [initial_population_file]" << endl;
		return 1;
	}

	// Copying the build note to the current dir
	{
		ifstream source(string(argv[0]) + ".note", ios::binary);
		ofstream dest("build.note", ios::binary);
		dest << source.rdbuf();
		source.close(); dest.close();
	}

	// Parsing and saving random seeds and initial population file name
	const unsigned evolutionSeed = stoul(argv[1]);
	const string initialPopulationFilename = argc==3 ? argv[2] : "";

	ofstream seedStream("seeds.ssv", ofstream::out);
	seedStream << " evolution_random_seed:" << evolutionSeed << endl;
	seedStream.close();

	// Creating an evolver and evolving
	cout << "Creating an evolver" << endl;

	MAFPOEvolver<TestIndividual> evolver(evolutionSeed, initialPopulationFilename, EVALUATIONS_TO_OPTIMIZE);

	cout << "Evolving..." << endl;

	evolver.evolve();

	cout << "...done evolving" << endl;

	return 0;
}
