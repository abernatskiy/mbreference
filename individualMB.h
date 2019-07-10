#pragma once

#include "constants.h"

#include "evolution/individual.h"
#include "mb/mb.h"
#include "task/digits5by5.h"
#include "constants.h"

class MarkovBrainIndividual : public Individual {
private:
	MarkovBrain mb;

	// All task-specific macros should be confined in this section
	static const std::vector<std::vector<bool>> percepts DIGITS5X5PERCEPTS;
	static const std::vector<unsigned> labels DIGITS5X5LABELS;
	static const unsigned numSensors = DIGITS5X5PERCEPTSIZE;
	static const unsigned numMotors = DIGITS5X5NUMDISTINCTLABELS*(1+INDIVIDUAL_MB_VETO_BITS);
	static const unsigned numHidden = INDIVIDUAL_MB_HIDDEN_NODES;
	static const unsigned numStates = numSensors + numMotors + numHidden;
	static const UIntRange mbInputsRange {0, numStates-1};
	static const UIntRange mbOutputsRange {numSensors, numStates-1};

	bool states[numStates];
	bool newStates[numStates];

public:
	std::string genomeStr() const override;
	void loadGenomeStr(std::string str) override;
	void randomize(std::mt19937_64& rng) override;
	bool mutate(std::mt19937_64& rng) override;
	void evaluate() override;
};
