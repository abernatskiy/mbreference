#pragma once

#include <iostream> // for now for debug only

#include <vector>
#include <utility>
#include <random>

#include "../utils/json11/json11.hpp"

typedef std::pair<unsigned,unsigned> UIntRange;

/********** Auxiliary functions **********/

unsigned upow2(unsigned power) {
	return 1u << power;
};

unsigned decodeUInt(const std::vector<bool>& bits) {
	unsigned answer = 0;
	for(const auto& bit : bits) {
		answer <<= 1;
		answer += (bit ? 1 : 0);
	}
	return answer;
};

unsigned sampleFromUIntRange(const UIntRange& range, std::mt19937_64& rng) {
	return std::uniform_int_distribution<unsigned>(range.first, range.second)(rng);
};

unsigned sampleUInt(unsigned maxVal, std::mt19937_64& rng) {
	return std::uniform_int_distribution<unsigned>(0, maxVal)(rng);
};

bool sampleBit(std::mt19937_64& rng) {
	return std::bernoulli_distribution(0.5)(rng);
};

/********** Gate class declaration/definition **********/

class Gate {
public:
	unsigned id;
	std::vector<unsigned> inputShifts;
	std::vector<unsigned> outputShifts;
	std::vector<std::vector<bool>> table;

	void update(bool* states, bool* newStates) const {
		std::vector<bool> key(inputShifts.size());
		for(unsigned i=0; i<inputShifts.size(); i++)
			key[i] = states[inputShifts[i]];
		unsigned ukey = decodeUInt(key);
		for(unsigned j=0; j<outputShifts.size(); j++)
			newStates[outputShifts[j]] = newStates[outputShifts[j]] || table[ukey][j];
	};

	json11::Json to_json() const {
		std::vector<int> convertedInputShifts(inputShifts.size());
		std::vector<int> convertedOutputShifts(outputShifts.size());
		std::copy(inputShifts.begin(), inputShifts.end(), convertedInputShifts.begin());
		std::copy(outputShifts.begin(), outputShifts.end(), convertedOutputShifts.begin());

		std::vector<std::vector<int>> convertedTable;
		for(const auto& row : table) {
			convertedTable.push_back({});
			for(const auto& val : row)
				convertedTable.back().push_back(val ? 1 : 0);
		}
		json11::Json out = json11::Json::object { { "id", json11::Json(static_cast<int>(id)) },
		                                          { "inputs", json11::Json(convertedInputShifts) },
		                                          { "outputs", json11::Json(convertedOutputShifts) },
		                                          { "table", json11::Json(convertedTable) },
		                                          { "type", "Deterministic" } };
		return out;
	};

	void from_json(const json11::Json& json) {
		id = json["id"].int_value();
		for(const auto& inshiftjson : json["inputs"].array_items())
			inputShifts.push_back(inshiftjson.int_value());
		for(const auto& outshiftjson : json["outputs"].array_items())
			outputShifts.push_back(outshiftjson.int_value());
		for(const auto& tablerowjson : json["table"].array_items()) {
			table.push_back({});
			for(const auto& tablevaljson : tablerowjson.array_items())
				table.back().push_back(tablevaljson.int_value()>0);
		}
	};

	void randomize(const UIntRange& inputNumRange,
	               const UIntRange& inputStatesRange,
	               const UIntRange& outputNumRange,
	               const UIntRange& outputStatesRange,
	               std::mt19937_64& rng) {
		const unsigned numInputs = sampleFromUIntRange(inputNumRange, rng);
		for(unsigned i=0; i<numInputs; i++)
			inputShifts.push_back(sampleFromUIntRange(inputStatesRange, rng));

		const unsigned numOutputs = sampleFromUIntRange(outputNumRange, rng);
		for(unsigned j=0; j<numOutputs; j++)
			outputShifts.push_back(sampleFromUIntRange(outputStatesRange, rng));

		// NOTE: there's no protection from reading from or writing into the same state node multiple times
		// It is the same in MABE (mabe/Brain/DEMarkovBrain/DEMarkovBrain.cpp lines 292, 294 @ 9dc4915)

		for(unsigned i=0; i<upow2(numInputs); i++) {
			table.push_back({});
			for(unsigned j=0; j<numOutputs; j++)
				table.back().push_back(sampleBit(rng));
		}
	};

	void rewireAConnectionRandomly(const UIntRange& inputStatesRange, const UIntRange& outputStatesRange, std::mt19937_64& rng) {
		unsigned rawidx = sampleUInt(inputShifts.size() + outputShifts.size() - 1, rng);
		if(rawidx < inputShifts.size())
			inputShifts[rawidx] = sampleFromUIntRange(inputStatesRange, rng);
		else
			outputShifts[rawidx-inputShifts.size()] = sampleFromUIntRange(outputStatesRange, rng);

		// NOTE: there's no protection from mutation not modifying the same connection if it samples
		// the same number as was there before. It's the same in MABE (mabe/Brain/MarkovBrain/Gate/AbstractGate.cpp @ mutateConnections())
	};

	void modifyTableRandomly(std::mt19937_64& rng) {
		const unsigned row = sampleUInt(table.size()-1, rng);
		const unsigned column = sampleUInt(outputShifts.size()-1, rng);
		table[row][column] = table[row][column] ? false : true;

		// NOTE: simple bitflip is the same as in MABE (mabe/Brain/MarkovBrain/Gate/DeterministicGate.cpp @ mutateInternalStructure())
	};
};
