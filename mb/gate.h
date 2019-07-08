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
	std::vector<bool*> inputs;
	std::vector<bool*> outputs;
	std::vector<std::vector<bool>> table;

	void update() {
		std::vector<bool> key(inputs.size());
		for(unsigned i=0; i<inputs.size(); i++)
			key[i] = *inputs[i];
		unsigned ukey = decodeUInt(key);
		for(unsigned j=0; j<outputs.size(); j++)
			*outputs[j] = table[ukey][j];
	};

	json11::Json to_json(bool* states) const {
		std::vector<int> inshifts, outshifts;
		for(const auto& in : inputs)
			inshifts.push_back(in-states);
		for(const auto& out : outputs)
			outshifts.push_back(out-states);
		std::vector<std::vector<int>> convertedTable;
		for(const auto& row : table) {
			convertedTable.push_back({});
			for(const auto& val : row)
				convertedTable.back().push_back(val ? 1 : 0);
		}
		json11::Json out = json11::Json::object { { "id", json11::Json(static_cast<int>(id)) },
		                                          { "inputs", json11::Json(inshifts) },
		                                          { "outputs", json11::Json(outshifts) },
		                                          { "table", json11::Json(convertedTable) },
		                                          { "type", "Deterministic" } };
		return out;
	};

	void from_json(const json11::Json& json, bool* states) {
		id = json["id"].int_value();
		for(const auto& inshiftjson : json["inputs"].array_items())
			inputs.push_back( states + inshiftjson.int_value() );
		for(const auto& outshiftjson : json["outputs"].array_items())
			outputs.push_back( states + outshiftjson.int_value() );
		for(const auto& tablerowjson : json["table"].array_items()) {
			table.push_back({});
			for(const auto& tablevaljson : tablerowjson.array_items())
				table.back().push_back( tablevaljson.int_value() == 1 );
		}
	};

	void randomize(bool* states,
	               unsigned numStates,
	               const UIntRange& inputNumRange,
	               const UIntRange& outputNumRange,
	               const UIntRange& outputStatesRange, // to be able to avoid wasting time writing into inputs
	               std::mt19937_64& rng) {
		const unsigned numInputs = sampleFromUIntRange(inputNumRange, rng);
		for(unsigned i=0; i<numInputs; i++)
			inputs.push_back(states + sampleUInt(numStates, rng));

		const unsigned numOutputs = sampleFromUIntRange(outputNumRange, rng);
		for(unsigned j=0; j<numOutputs; j++)
			outputs.push_back(states + sampleFromUIntRange(outputStatesRange, rng));

		// NOTE: there's no protection from reading from or writing into the same state node multiple times
		// It is the same in MABE (mabe/Brain/DEMarkovBrain/DEMarkovBrain.cpp lines 292, 294 @ 9dc4915)

		for(unsigned i=0; i<upow2(numInputs); i++) {
			table.push_back({});
			for(unsigned j=0; j<numOutputs; j++)
				table.back().push_back(sampleBit(rng));
		}
	};
};
