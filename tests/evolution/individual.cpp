#include <iostream>
#include <vector>

#include "../../evolution/individual.h"

using namespace std;

int main() {
	cout << "Generating a bunch of random TestIndividuals. Results:" << endl;

	vector<TestIndividual> indivs(5);

	mt19937_64 rng(43);
	for(auto&& i : indivs)
		i.randomize(rng);

	for(auto i : indivs)
		cout << i.str() << endl;

	cout << endl;

	cout << "Mutating the individuals. Results:" << endl;
	for(auto&& i : indivs) {
		bool mutated = i.mutate(rng);
		cout << ( mutated?"m ":"n " ) << i.str() << endl;
	}

	cout << endl;

	cout << "Evaluating the individuals. Results:" << endl;
	for(auto&& i : indivs)
		i.evaluate();

	for(auto i : indivs)
		cout << i.str() << endl;

	cout << endl;

	cout << "Comparing the individuals' errors" << endl;

	for(auto first : indivs)
		for(const auto& second : indivs) {
			bool better = first.isBetterThan(second, "error");
			bool equiv = first.isEquivalentTo(second, "error");
			cout << first.str() << " vs " <<  second.str() << ": better " << (better?"y":"n") << " equivalent " << (equiv?"y":"n") << endl;
		}

	cout << endl;

	cout << "Assigning additional objective of age to the individuals and studying dominance" << endl;

	{
		unsigned curage = 0;
		for(auto&& first : indivs) {
			first.evals["age"] = curage;
			curage++;
		}
	}

	cout << endl;

	for(auto first : indivs)
		for(const auto& second : indivs) {
			bool dom = first.isAllEquivalentTo(second, {"error", "age"});
			cout << first.str() << " is " << (dom?"":"not ") << "equivalent to " << second.str() << endl;
		}

	cout << endl;

	for(auto first : indivs)
		for(const auto& second : indivs) {
			bool dom = first.isAnyBetterThan(second, {"error", "age"});
			cout << first.str() << " is " << (dom?"":"not any ") << "better than " << second.str() << endl;
		}

	cout << endl;

	for(auto first : indivs)
		for(const auto& second : indivs) {
			bool dom = first.isInNoWayWorseThan(second, {"error", "age"});
			cout << first.str() << " is " << (dom?"in no way worse than ":"in some way worse than ") << second.str() << endl;
		}

	cout << endl;

	for(auto first : indivs)
		for(const auto& second : indivs) {
			bool dom = first.isDominatedBy(second, {"error", "age"});
			cout << first.str() << " is " << (dom?"":"not ") << "dominated by " << second.str() << endl;
		}

	cout << endl;

	cout << "Making representations:" << endl;

	vector<string> reprs;

	for(auto i : indivs)
		reprs.push_back(i.str());
	for(auto s : reprs)
		cout << s << endl;

	cout << endl;

	cout << "Restoring the individuals from the representations. The restored ones:" << endl;

	for(auto s : reprs) {
		TestIndividual tsti;
		tsti.loadStr(s);
		cout << tsti.str() << endl;
	}

	return 0;
}
