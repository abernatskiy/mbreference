#include <iostream>

#include "../../evolution/evolverMAFPO.h"

int main() {

	MAFPOEvolver<TestIndividual> evolver(42, "", {"error"});

	evolver.evolve();

	return 0;
}
