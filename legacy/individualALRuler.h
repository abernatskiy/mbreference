#pragma once

#include "constants.h"

#include "evolution/individual.h"
#include "alruler.h"
#include "alrulerevaluator.h"

#ifndef ADD_REM_RATIO
#define ADD_REM_RATIO 1.0
#endif // ADD_REM_RATIO

#ifndef MOD_FRACTION
#define MOD_FRACTION 0.8
#endif // MOD_FRACTION

#define REM_FRACTION ((1.0-MOD_FRACTION)/(1.+ADD_REM_RATIO))

class ALRulerIndividual : public Individual {
private:
	ALRuler ruler;

	void resetANN();
	double getRandomWeight(std::mt19937_64& rng) const;
	double modifyWeight(double oldWeight, std::mt19937_64& rng) const;

	unsigned countConnections(unsigned layer) const;
	std::vector<std::pair<unsigned,unsigned>> allLayerConnections(unsigned layer) const;
	std::vector<std::pair<unsigned,unsigned>> allLayerDisconnections(unsigned layer) const;

	bool addRandomConnections(unsigned numConnections, unsigned layer, std::mt19937_64& rng);
	bool modifyRandomConnections(unsigned numConnections, unsigned layer, std::mt19937_64& rng);
	bool removeRandomConnections(unsigned numConnections, unsigned layer, std::mt19937_64& rng);

public:
	std::string genomeStr() const override;
	void loadGenomeStr(std::string str) override;
	void randomize(std::mt19937_64& rng) override;
	bool mutate(std::mt19937_64& rng) override;
	void evaluate() override;

	void drawANNWeights() { ruler.ann.drawWeights(); };
};
