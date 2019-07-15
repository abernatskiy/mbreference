#include <iostream>
#include <cstdlib>

#include "utils/json11/json11.hpp"

#include "individualMB.h"

using namespace std;
using namespace json11;

const std::vector<std::vector<bool>> MarkovBrainIndividual::percepts DIGITS5X5PERCEPTS;
const std::vector<unsigned> MarkovBrainIndividual::labels DIGITS5X5LABELS;
const UIntRange MarkovBrainIndividual::mbInputsRange {0, _INDIVIDUAL_MB_NUM_TOTAL-1};
const UIntRange MarkovBrainIndividual::mbOutputsRange {_INDIVIDUAL_MB_NUM_SENSORS, _INDIVIDUAL_MB_NUM_TOTAL-1};

/********** Public definitions ***********/

string MarkovBrainIndividual::genomeStr() const {
	Json::object indivJSON { { "brains", Json::object {{ "root::", mb.to_json() }} },
	                         { "data_map", evals },
	                         { "genomes", Json::array() },
	                         { "id", static_cast<int>(id) }
//	                         { "parent_ids", Json::array() } // omitting the field to make it clear that parent tracking is not yet implemented
	                       };
	return Json(indivJSON).dump();

}

void MarkovBrainIndividual::loadGenomeStr(string str) {
	string parseError;
	Json indivJSON = Json::parse(str, parseError);

	if(!parseError.empty()) {
		cerr << "Individual JSON parsing failed with message " << parseError << endl;
		exit(EXIT_FAILURE);
	}

	mb.from_json(indivJSON["brains"]["root::"], true);
	if(!mb.validateMABEMetadata(_INDIVIDUAL_MB_NUM_SENSORS, _INDIVIDUAL_MB_NUM_MOTORS, _INDIVIDUAL_MB_NUM_HIDDEN)) {
		cerr << "Disagreement between the loaded brain state ranges and mbreference task settings detected, exiting" << endl;
		exit(EXIT_FAILURE);
	}
	id = indivJSON["id"].int_value();
	evals["negScore"] = indivJSON["data_map"]["negScore"].number_value(); // NOTE: might want to add more or even all of them
	evals["mbg"] = indivJSON["data_map"]["mbg"].number_value();
}

void MarkovBrainIndividual::randomize(mt19937_64& rng) {
	mb.makeRandom(mbInputsRange, mbOutputsRange, INDIVIDUAL_MB_INITIAL_GATES, rng);
	mb.saveMABEMetadata(_INDIVIDUAL_MB_NUM_SENSORS, _INDIVIDUAL_MB_NUM_MOTORS, _INDIVIDUAL_MB_NUM_HIDDEN);
}

bool MarkovBrainIndividual::mutate(mt19937_64& rng) {
	mb.mutate(mbInputsRange, mbOutputsRange, rng);
	renewId();
	return true; // TODO: consider the consequences of this
}

void MarkovBrainIndividual::evaluate() {

	double score = 0.;
	for(unsigned perceptIdx=0; perceptIdx<percepts.size(); perceptIdx++) {

		// Simulating brain for a while, showing it the current percept
		fill(states, states+_INDIVIDUAL_MB_NUM_TOTAL, false);
		for(unsigned t=0; t<=INDIVIDUAL_MB_TIME_STEPS; t++) {
			fill(newStates, newStates+_INDIVIDUAL_MB_NUM_TOTAL, false);
			copy(percepts[perceptIdx].begin(), percepts[perceptIdx].end(), states); // not to newStates cause we want the brain to see right away
			mb.update(states, newStates);
			copy(newStates, newStates+_INDIVIDUAL_MB_NUM_TOTAL, states);
			// TODO: add break condition here if trigger bits are used
		}

		// Reading outputs and determining the numbers that were output
		bool* outputs = states + _INDIVIDUAL_MB_NUM_SENSORS;
		bool rightAnswerGiven = false;
		unsigned numAnswers = 0;
		unsigned curLabel = 0;
		for(bool* curOut=states+_INDIVIDUAL_MB_NUM_SENSORS; curOut<states+_INDIVIDUAL_MB_NUM_SENSORS+_INDIVIDUAL_MB_NUM_MOTORS; curOut+=(INDIVIDUAL_MB_VETO_BITS+1)) {
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
