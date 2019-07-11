#include <iostream>
#include <vector>

#define EVOLUTION_RANDOM_SEED 42324

#include "../individualMB.h"

using namespace std;

int main() {

	cout << "Generating a bunch of random MarkovBrainIndividuals. Results:" << endl;

	vector<MarkovBrainIndividual> indivs(10);

	mt19937_64 rng(EVOLUTION_RANDOM_SEED);
	for(auto&& i : indivs)
		i.randomize(rng);

	for(const auto& i : indivs)
		cout << i.str() << endl;

//	for(auto i : indivs) {
//		cout << "Individual " << i.id << ":" << endl;
//		i.drawANNWeights();
//	}

	cout << endl;

	cout << "Mutating the individuals..." << endl;
	for(auto&& i : indivs) {
		bool mutated = i.mutate(rng);
		cout << "Individual " << i.id << ( mutated?" ":"not " ) << "mutated" << endl;
	}

//	for(auto i : indivs) {
//		cout << "Individual " << i.id << ":" << endl;
//		i.drawANNWeights();
//	}

	cout << endl;

	cout << "Evaluating the individuals. Results:" << endl;
	for(auto&& i : indivs)
		i.evaluate();

	for(const auto& i : indivs)
		cout << i.str() << endl;

	cout << endl;

	cout << "Serializing the individuals and then deserealizing them. Result:" << endl;

	vector<MarkovBrainIndividual> deserializedIndivs;
	for(auto i : indivs) {
		string s = i.str();
		MarkovBrainIndividual alri;
		alri.loadStr(s);
		deserializedIndivs.push_back(alri);
	}

	for(const auto& i : deserializedIndivs)
		cout << i.str() << endl;

	return 0;
}
