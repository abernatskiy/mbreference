#pragma once

// #define EVALUATIONS_TO_OPTIMIZE {"avgDist", "stuckness"}
#define EVALUATIONS_TO_OPTIMIZE {"avgDist"}

#define EVOLVER_MAX_GENERATIONS 200 // -1 for indefinite
#define EVOLVER_POPULATION_SIZE 100

#define EVOLVER_EVALUATIONS_OF_INTEREST {"error", "avgDist", "stuckness", "wandering"}

#define EVOLVER_RECORDS_PERIOD 100 // -1 for never
#define EVOLVER_EVALUATIONS_OF_INTEREST_PERIOD 1 // -1 for never
#define EVOLVER_EVALUATIONS_OF_INTEREST_INDEPENDENT false

#define EVOLVER_MAFPO_RECORDS_PERIOD 1 // -1 for never

#define DEBUG

#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif

#define NUM_THREADS 8
