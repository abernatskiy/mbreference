#include <iostream>
#include <cstdlib>

#include "../utils/randutils.h"
#include "mb.h"

using namespace std;
using namespace json11;

/********** Public definitions **********/

void MarkovBrain::update(bool* states, bool* newStates) const {
	for(const auto& g : gates)
		g.update(states, newStates);
}

void MarkovBrain::from_json(const Json& json, bool keepMetadata) {
	for(const auto& gateJSON : json["gates"].array_items()) {
		Gate g;
		g.from_json(gateJSON);
		if(g.id >= nextGateID)
			nextGateID = g.id+1;
		gates.push_back(g);
	}

	if(keepMetadata) {
		for(const auto& objpair : json.object_items())
			if(objpair.first != "gates")
				metadata[objpair.first] = Json(objpair.second);
	}
}

void MarkovBrain::from_json_str(string jsonStr, bool keepMetadata) {
	string errorMessage;
	Json parsedJson = Json::parse(jsonStr, errorMessage);
	if(!errorMessage.empty()) {
		cerr << "MarkovBrain::from_json_str: JSON parsing failed, message was " << errorMessage << endl;
		exit(EXIT_FAILURE);
	}
	from_json(parsedJson, keepMetadata);
}

Json MarkovBrain::to_json() const {
	vector<Json> gateJSONs;
	for(const auto& g : gates)
		gateJSONs.push_back(g.to_json());

	map<string,Json> out { { "gates", gateJSONs } };
	for(const auto& mdpair : metadata)
		out[mdpair.first] = mdpair.second;

	return Json(out);
}

std::string MarkovBrain::to_json_str() const {
	return to_json().dump();
}

void MarkovBrain::makeRandom(const UIntRange& inputsRange, const UIntRange& outputsRange, unsigned numGates, mt19937_64& rng) {
	gates.clear();
	nextGateID = 0;

	const UIntRange inputsNumRange MB_GATES_NUM_INPUTS;
	const UIntRange outputsNumRange MB_GATES_NUM_OUTPUTS;
	for(unsigned i=0; i<numGates; i++) {
		Gate g;
		g.id = nextGateID++;
		g.randomize(inputsNumRange, inputsRange, outputsNumRange, outputsRange, rng);
		gates.push_back(g);
	}
}

void MarkovBrain::mutate(const UIntRange& inputsRange, const UIntRange& outputsRange, mt19937_64& rng) {

	const UIntRange inputsNumRange MB_GATES_NUM_INPUTS;
	const UIntRange outputsNumRange MB_GATES_NUM_OUTPUTS;

	const double insertionThreshold = MB_GATE_INSERTION_PROB;
	const double deletionThreshold = insertionThreshold + MB_GATE_DELETION_PROB;
	const double duplicationThreshold = deletionThreshold + MB_GATE_DUPLICATION_PROB;
	const double tableModificationThreshold = duplicationThreshold + (1.-duplicationThreshold) / (1.+MB_GATE_CONNECTION_TO_TABLE_CHANGE_RATIO);

	const double r = sampleZeroToOneDouble(rng);
//	D( cout << "r=" << r << endl; )
	if(r < insertionThreshold) {
		Gate g;
		g.id = nextGateID++;
		g.randomize(inputsNumRange, inputsRange, outputsNumRange, outputsRange, rng);
		gates.push_back(g);
//		D( cout << "Inserted a gate" << endl; )
	}
	else {
		if(gates.size()<1) {
			mutate(inputsRange, outputsRange, rng);
			return;
		}
		unsigned idx = sampleUInt(gates.size()-1, rng);
		if(r < deletionThreshold) {
			gates.erase(gates.begin()+idx);
//			D( cout << "Removed gate " << idx <<  endl; )
		}
		else if(r < duplicationThreshold) {
			Gate g = gates[idx];
			g.id = nextGateID++;
			gates.push_back(g);
//			D( cout << "Duplicated gate " << idx << endl; )
		}
		else if(r < tableModificationThreshold) {
			gates[idx].modifyTableRandomly(rng);
//			D( cout << "Modified table of gate " << idx << endl; )
		}
		else {
			gates[idx].rewireAConnectionRandomly(inputsRange, outputsRange, rng);
//			D( cout << "Rewired a connection of gate " << idx << endl; )
		}
	}
}

void MarkovBrain::saveMABEMetadata(unsigned numInputs, unsigned numOutputs, unsigned numHidden) {
	metadata["numInputs"] = static_cast<int>(numInputs);
	metadata["numOutputs"] = static_cast<int>(numOutputs);
	metadata["numHidden"] = static_cast<int>(numHidden);
}

bool MarkovBrain::validateMABEMetadata(unsigned numInputs, unsigned numOutputs, unsigned numHidden) const {
	if( metadata.find("numInputs")==metadata.end() ||
	    metadata.find("numOutputs")==metadata.end() ||
	    metadata.find("numHidden")==metadata.end() )
		return false;

	return metadata.at("numInputs").int_value() == numInputs &&
	       metadata.at("numOutputs").int_value() == numOutputs &&
	       metadata.at("numHidden").int_value() == numHidden;
}
