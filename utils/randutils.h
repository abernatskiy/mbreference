#pragma once

typedef std::pair<unsigned,unsigned> UIntRange;

inline unsigned sampleFromUIntRange(const UIntRange& range, std::mt19937_64& rng) {
	return std::uniform_int_distribution<unsigned>(range.first, range.second)(rng);
};

inline unsigned sampleUInt(unsigned maxVal, std::mt19937_64& rng) {
	return std::uniform_int_distribution<unsigned>(0, maxVal)(rng);
};

inline bool sampleBit(std::mt19937_64& rng) {
	return std::bernoulli_distribution(0.5)(rng);
};

inline double sampleZeroToOneDouble(std::mt19937_64& rng) {
	return std::uniform_real_distribution<double>(0., 1.)(rng);
};
