#include <iostream>
#include <vector>

#define EVOLUTION_RANDOM_SEED 44
#define TRAINING_SET_RANDOM_SEED 42

#include "../individualALRuler.h"
#include "../alssetgen.h"

using namespace std;

ALRulerEvaluator rulerEvaluator;

int main() {

	// Creating a training set and an evaluator
	shared_ptr<vector<pair<ALStatement,ALStatement>>> trainingSetPtr =
		make_shared<vector<pair<ALStatement,ALStatement>>>(randomALStatementPairs(TRAINING_SET_SIZE, TRAINING_SET_RANDOM_SEED));
	// allALStatementPairs() with default settings produces 414*414=171396 pairs

	cout << "Created a training set consisting of " << trainingSetPtr->size() << " pairs" << endl;

	rulerEvaluator = ALRulerEvaluator(trainingSetPtr); // rulerEvaluator must be set before doing any evaluations on these individuals

	cout << "Generating a bunch of random ALRulerIndividuals. Results:" << endl;

	vector<ALRulerIndividual> indivs(5);

	mt19937_64 rng(EVOLUTION_RANDOM_SEED);
	for(auto&& i : indivs)
		i.randomize(rng);

//	for(auto i : indivs)
//		cout << i.str() << endl;


	for(auto i : indivs) {
		cout << "Individual " << i.id << ":" << endl;
		i.drawANNWeights();
	}

	cout << endl;

	cout << "Mutating the individuals..." << endl;
	for(auto&& i : indivs) {
		bool mutated = i.mutate(rng);
		cout << "Individual " << i.id << ( mutated?" ":"not " ) << "mutated" << endl;
	}

	for(auto i : indivs) {
		cout << "Individual " << i.id << ":" << endl;
		i.drawANNWeights();
	}

	cout << endl;

	cout << "Evaluating the individuals. Results:" << endl;
	for(auto&& i : indivs)
		i.evaluate();

	for(auto i : indivs)
		cout << i.str() << endl;

	cout << endl;

	cout << "Serializing the individuals and then deserealizing them. Result:" << endl;

	vector<ALRulerIndividual> deserializedIndivs;
	for(auto i : indivs) {
		string s = i.str();
		ALRulerIndividual alri;
		alri.loadStr(s);
		deserializedIndivs.push_back(alri);
	}

	for(auto i : deserializedIndivs)
		cout << i.str() << endl;

	return 0;
}
