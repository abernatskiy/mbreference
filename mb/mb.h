#pragma once

#include <vector>
#include <map>
#include <random>
#include <string>

#include "../utils/json11/json11.hpp"
#include "gate.h"
#include "../constants.h"

class MarkovBrain {
private:
	unsigned nextGateID;
	std::vector<Gate> gates;
	std::map<std::string,json11::Json> metadata; // holds the data that is required to stay compatible with MABE

public:
	MarkovBrain() : nextGateID(0) {};
	unsigned numGates() const { return gates.size(); };
	void update(bool* states, bool* newStates) const;
	void from_json(const json11::Json& json, bool keepMetadata=false);
	void from_json_str(std::string jsonStr, bool keepMetadata=false);
	json11::Json to_json() const;
	std::string to_json_str() const;
	void makeRandom(const UIntRange& inputsRange, const UIntRange& outputsRange, unsigned numGates, std::mt19937_64& rng);
//	void mutate(const UIntRange& inputsRange, const UIntRange& outputsRange, std::mt19937_64& rng);
};
