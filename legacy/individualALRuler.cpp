#include <sstream>
#include <iomanip>
#include <set>

#include "individualALRuler.h"

// Globals

extern ALRulerEvaluator rulerEvaluator;

// Private definitions

double ALRulerIndividual::getRandomWeight(std::mt19937_64& rng) const {
	static std::uniform_real_distribution<double> weightDistribution(ANN_INITIAL_WEIGHTS_MIN, ANN_INITIAL_WEIGHTS_MAX);
	return weightDistribution(rng);
}

double ALRulerIndividual::modifyWeight(double oldWeight, std::mt19937_64& rng) const {
	std::normal_distribution<double> weightDistribution(oldWeight, oldWeight);
	return weightDistribution(rng);
}

bool ALRulerIndividual::addRandomConnections(unsigned numConnections, unsigned layer, std::mt19937_64& rng) {
	auto potentialConnections = ruler.ann.allDisconnectionsIndices(layer);
	if(potentialConnections.size()<numConnections)
		return false;
	std::uniform_int_distribution<unsigned> connectionPicker(0, potentialConnections.size()-1);
	std::set<std::pair<unsigned,unsigned>> newConnections;
	while(newConnections.size()<numConnections)
		newConnections.insert(potentialConnections[connectionPicker(rng)]);

//	std::cerr << "Adding connections:";

	for(auto nc : newConnections) {
		unsigned i, j;
		std::tie(i, j) = nc;
		ruler.ann.weights[layer][i][j] = getRandomWeight(rng);

//		std::cerr << " " << i << "," << j;
	}

//	std::cerr << std::endl;

	return true;
}

bool ALRulerIndividual::modifyRandomConnections(unsigned numConnections, unsigned layer, std::mt19937_64& rng) {
	auto potentialConnections = ruler.ann.allConnectionsIndices(layer);
	if(potentialConnections.size()<numConnections)
		return false;
	std::uniform_int_distribution<unsigned> connectionPicker(0, potentialConnections.size()-1);
	std::set<std::pair<unsigned,unsigned>> newConnections;
	while(newConnections.size()<numConnections)
		newConnections.insert(potentialConnections[connectionPicker(rng)]);

//	std::cerr << "Modifying connections:";

	for(auto nc : newConnections) {
		unsigned i, j;
		std::tie(i, j) = nc;
		ruler.ann.weights[layer][i][j] = modifyWeight(ruler.ann.weights[layer][i][j], rng);

//		std::cerr << " " << i << "," << j;
	}

//	std::cerr << std::endl;

	return true;
}

bool ALRulerIndividual::removeRandomConnections(unsigned numConnections, unsigned layer, std::mt19937_64& rng) {
	auto potentialDisconnections = ruler.ann.allConnectionsIndices(layer);
	if(potentialDisconnections.size()<numConnections)
		return false;
	std::uniform_int_distribution<unsigned> connectionPicker(0, potentialDisconnections.size()-1);
	std::set<std::pair<unsigned,unsigned>> newDisconnections;
	while(newDisconnections.size()<numConnections)
		newDisconnections.insert(potentialDisconnections[connectionPicker(rng)]);

//	std::cerr << "Removing connections:";

	for(auto nc : newDisconnections) {
		unsigned i, j;
		std::tie(i, j) = nc;
		ruler.ann.weights[layer][i][j] = 0.;

//		std::cerr << " " << i << "," << j;
	}

//	std::cerr << std::endl;

	return true;
}

// Public definitions

std::string ALRulerIndividual::genomeStr() const {
	return ruler.ann.compactStr();
}

void ALRulerIndividual::loadGenomeStr(std::string annStr) {
	ruler.ann.loadCompactStr(annStr);
}

void ALRulerIndividual::randomize(std::mt19937_64& rng) {
	ruler.ann.reset();
	addRandomConnections(2*ANN_HIDDEN_SIZE, 0, rng); // E-S W style networks - avg in-degree of two
	addRandomConnections(2*1, 1, rng);
}

bool ALRulerIndividual::mutate(std::mt19937_64& rng) {
	static const double inputLayerConnectionFraction = static_cast<double>((ANN_INPUT_SIZE+1)*ANN_HIDDEN_SIZE) /
		static_cast<double>((ANN_INPUT_SIZE+1)*ANN_HIDDEN_SIZE + (ANN_HIDDEN_SIZE+1)*1);
	static std::bernoulli_distribution layerDist(inputLayerConnectionFraction);
	unsigned curLayer = layerDist(rng) ? 0 : 1;

	static std::uniform_real_distribution<double> rdist(0., 1.);
	double r = rdist(rng);

	bool out;

//	D( std::cerr << "Mutating layer " << curLayer << " with r " << r << std::endl; )

	if(r<MOD_FRACTION)
		out = modifyRandomConnections(1, curLayer, rng);
	else if(r<MOD_FRACTION+REM_FRACTION)
		out = removeRandomConnections(1, curLayer, rng);
	else
		out = addRandomConnections(1, curLayer, rng);

	if(out) renewId();

	return out;
}

void ALRulerIndividual::evaluate() {
	auto rawEvals = rulerEvaluator.evaluateRuler(ruler);
	for(auto e : rawEvals) {
//		std::cerr << "Individual " << id << " got evals: " << e.first << " " << e.second << std::endl;
		evals[e.first] = e.second;
	}
}
