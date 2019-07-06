#include <iostream>

// Make sure that the following value is as below in constants.h
// #define EVOLVER_EVALUATIONS_OF_INTEREST {"error"}

#include "../../evolution/evolver.h"

int main() {

	TestEvolver<TestIndividual> evolver("error", 442);

	evolver.evolve();

	return 0;
}
