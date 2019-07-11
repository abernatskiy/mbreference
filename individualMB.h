#pragma once

#include "constants.h"

#include "evolution/individual.h"
#include "mb/mb.h"
#include "task/digits5by5.h"
#include "constants.h"

#define _INDIVIDUAL_MB_NUM_SENSORS DIGITS5X5PERCEPTSIZE
#define _INDIVIDUAL_MB_NUM_MOTORS DIGITS5X5NUMDISTINCTLABELS*(1+INDIVIDUAL_MB_VETO_BITS)
#define _INDIVIDUAL_MB_NUM_HIDDEN INDIVIDUAL_MB_HIDDEN_NODES
#define _INDIVIDUAL_MB_NUM_TOTAL _INDIVIDUAL_MB_NUM_SENSORS+_INDIVIDUAL_MB_NUM_MOTORS+_INDIVIDUAL_MB_NUM_HIDDEN

class MarkovBrainIndividual : public Individual {
private:
	MarkovBrain mb;

	static const std::vector<std::vector<bool>> percepts; // initialized to DIGITS5X5PERCEPTS in individualMB.cpp
	static const std::vector<unsigned> labels; // initialized to DIGITS5X5LABELS in individualMB.cpp
	static const UIntRange mbInputsRange; // initialized to {0, _INDIVIDUAL_MB_NUM_TOTAL-1}
	static const UIntRange mbOutputsRange; // initialized to {_INDIVIDUAL_MB_NUM_SENSORS, _INDIVIDUAL_MB_NUM_TOTAL-1}

	bool states[_INDIVIDUAL_MB_NUM_TOTAL];
	bool newStates[_INDIVIDUAL_MB_NUM_TOTAL];

public:
	std::string genomeStr() const override;
	void loadGenomeStr(std::string str) override;
	void randomize(std::mt19937_64& rng) override;
	bool mutate(std::mt19937_64& rng) override;
	void evaluate() override;
};
