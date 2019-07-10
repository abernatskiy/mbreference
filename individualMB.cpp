#include <iostream>
#include <cstdlib>

#include "utils/json11/json11.hpp"

#include "individualMB.h"

using namespace std;
using namespace json11;

/********** Public definitions ***********/

string MarkovBrainIndividual::genomeStr() const override {
	Json indivJSON { { "brains", { "root::", mb.to_json() } },
	                 { "data_map", evals },
	                 { "genomes", Json::array() },
	                 { "id", id }
	//                 { "parent_ids", Json::array() } // omitting the field to make it clear that parent tracking is not yet implemented
	               };
	return indivJSON.dump();

}

void MarkovBrainIndividual::loadGenomeStr(string str) override {
	Json indivJSON = Json::parse(str);
	mb.from_json(indivJSON["brains"]["root::"], true);
	if(!mb.validateMABEMetadata(numSensors, numMotors, numHidden)) {
		cerr << "Disagreement between the loaded brain state ranges and mbreference task settings detected, exiting" << endl;
		exit(EXIT_FAILURE);
	}
	id = indivJSON["id"].int_value();
	evals["negScore"] = indivJSON["data_map"]["negScore"].number_value(); // NOTE: might want to add more or even all of them
	evals["mbg"] = indivJSON["data_map"]["mbg"].number_value();
}

void MarkovBrainIndividual::randomize(mt19937_64& rng) override {
	mb.makeRandom(mbInputsRange, mbOutputsRange, INDIVIDUAL_MB_INITIAL_GATES, rng);
	mb.saveMABEMetadata(numSensors, numMotors, numHidden);
}

bool MarkovBrainIndividual::mutate(mt19937_64& rng) override {
	mb.mutate(mbInputsRange, mbOutputsRange, rng);
	return true; // TODO: consider the consequences of this
}

void MarkovBrainIndividual::evaluate() override {

	double score = 0.;
	for(unsigned perceptIdx=0; perceptIdx<percepts.size(); perceptIdx++) {

		// Simulating brain for a while, showing it the current percept
		fill(states, states+numStates, false);
		for(unsigned t=0; t<=INDIVIDUAL_MB_TIME_STEPS; t++) {
			fill(newStates, newStates+numStates, false);
			copy(percepts[perceptIdx].begin(), percepts[perceptIdx].end(), states); // not to newStates cause we want the brain to see right away
			mb.update(states, newStates);
			copy(newStates, newStates+numStates, states);
			// TODO: add break condition here if trigger bits are used
		}

		// Reading outputs and determining the numbers that were output
		bool* outputs = states + numSensors;
		bool rightAnswerGiven = false;
		unsigned numAnswers = 0;
		unsigned curLabel = 0;
		for(bool* curOut=states+numSensors; curOut<states+numSensors+numMotors; curOut+=(INDIVIDUAL_MB_VETO_BITS+1)) {
			bool veto = false;
			for(unsigned i=0; i<INDIVIDUAL_MB_VETO_BITS; i++)
				veto = veto || (curOut[i]);
			if(!veto && curOut[INDIVIDUAL_MB_VETO_BITS]) {
				numAnswers++;
				if(curLabel==labels[perceptIdx])
					rightAnswerGiven = true;
			}
			curLabel++;
		}

		// If the right answer was among outputs, increase fitness by one over number of answers
		if(rightAnswerGiven)
			score += 1./static_cast<double>(numAnswers);
	}

	evals["negScore"] = -score;
	evals["mbg"] = mb.numGates();
}
