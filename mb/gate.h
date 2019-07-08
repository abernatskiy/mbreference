#pragma once

#include <vector>

/********** Auxiliary functions **********/

unsigned decodeUInt(const std::vector<bool>& bits) {
	unsigned answer = 0;
	for(const auto& bit : bits) {
		answer <<= 1;
		answer += (bit ? 1 : 0);
	}
	return answer;
};

/********** Gate class declaration/definition **********/

class Gate {
public:
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
};
