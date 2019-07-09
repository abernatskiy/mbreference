#pragma once

inline unsigned upow2(unsigned power) {
	return 1u << power;
};

inline unsigned decodeUInt(const std::vector<bool>& bits) {
	unsigned answer = 0;
	for(const auto& bit : bits) {
		answer <<= 1;
		answer += (bit ? 1 : 0);
	}
	return answer;
};
