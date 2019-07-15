#pragma once

// mb/gate.h definitions
#define MB_GATES_NUM_INPUTS {1, 4}
#define MB_GATES_NUM_OUTPUTS {1, 4}

// mb/mb.h|.cpp definitions
#define MB_GATE_INSERTION_PROB 0.1
#define MB_GATE_DELETION_PROB 0.1
#define MB_GATE_DUPLICATION_PROB 0.2
#define MB_GATE_CONNECTION_TO_TABLE_CHANGE_RATIO 1.0

// individualMB.h|.cpp definitions
#define INDIVIDUAL_MB_VETO_BITS 1
#define INDIVIDUAL_MB_TIME_STEPS 30
#define INDIVIDUAL_MB_HIDDEN_NODES 10
#define INDIVIDUAL_MB_INITIAL_GATES 10

// evolution/* definitions
#define EVALUATIONS_TO_OPTIMIZE {"negScore", "mbg"}

#define EVOLVER_MAX_GENERATIONS 2000 // -1 for indefinite
#define EVOLVER_POPULATION_SIZE 400

#define EVOLVER_EVALUATIONS_OF_INTEREST {"negScore", "mbg"}

#define EVOLVER_RECORDS_PERIOD 1000 // -1 for never
#define EVOLVER_EVALUATIONS_OF_INTEREST_PERIOD 1 // -1 for never
#define EVOLVER_EVALUATIONS_OF_INTEREST_INDEPENDENT false

#define EVOLVER_MAFPO_RECORDS_PERIOD 1 // -1 for never

#define BREAK_TIES_BY_ID true // the default is true

#define NUM_THREADS 10

// common macros
#define DEBUG

#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif
