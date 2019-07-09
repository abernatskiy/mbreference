#include "mb.h"

#include <iostream>
#include <cstdlib>

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
/*
void MarkovBrain::mutate(const UIntRange& inputsRange, const UIntRange& outputsRange, mt19937_64& rng) {

}
*/
