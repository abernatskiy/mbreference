#pragma once

#include <map>
#include <string>
#include <vector>
#include <random>

#include <iostream> // for debug
#include "../constants.h" // for debug macros

#ifndef BREAK_TIES_BY_ID
#define BREAK_TIES_BY_ID true
#endif // BREAK_TIES_BY_ID

std::string evaluationsToCompactString(std::map<std::string,double> evals);
std::map<std::string,double> compactStringToEvaluations(std::string str);

class Individual {
public:
	long unsigned id;
	std::map<std::string,double> evals; // evaluation names must not contain spaces

	Individual();
	virtual ~Individual() = default;

	void renewId();

	bool isEquivalentTo(const Individual& other, std::string evalName) const { return evals.at(evalName) == other.evals.at(evalName); };
	bool isBetterThan(const Individual& other, std::string evalName) const { return evals.at(evalName) < other.evals.at(evalName); };

	bool isAllEquivalentTo(const Individual& other, const std::vector<std::string>& evalNames) const;
	bool isAnyBetterThan(const Individual& other, const std::vector<std::string>& evalNames) const;
	bool isInNoWayWorseThan(const Individual& other, const std::vector<std::string>& evalNames) const;
	bool isDominatedBy(const Individual& other, const std::vector<std::string>& evalNames) const;

	std::string str() const { return std::to_string(id) + " " + evaluationsToCompactString(evals) + " " + genomeStr(); };
	void loadStr(std::string str); // calls loadGenomeStr() internally once it is done loading params and id

	virtual std::string genomeStr() const = 0; // no newlines; no spaces is generally a good idea, although not a must; preferred separator is comma
	virtual void loadGenomeStr(std::string genomeStr) = 0; // same as above, but in reverse
	virtual void randomize(std::mt19937_64& rng) = 0;
	virtual bool mutate(std::mt19937_64& rng) = 0; // return whether the mutation changed the individual; if it did, don't forget to renew id!
	virtual void evaluate() = 0; // must update the evals data member above
};

class TestIndividual : public Individual {
public:
	unsigned val;

	static const unsigned targetval = 77;
	static const unsigned maxval = 100;

	std::string genomeStr() const override { return std::to_string(val); };
	void loadGenomeStr(std::string genomeStr) override { val = std::stod(genomeStr); };

	void randomize(std::mt19937_64& rng) override {
		static std::uniform_int_distribution<unsigned> valdist(0, maxval);
		val = valdist(rng);
	};
	bool mutate(std::mt19937_64& rng) override {
		static std::uniform_int_distribution<unsigned> mutdist(0, 1);
		unsigned change = mutdist(rng);
		if(change==0 && val>0) {
			val -= 1;
			renewId();
			return true;
		}
		else if(change==1 && val<maxval) {
			val += 1;
			renewId();
			return true;
		}
		else
			return false;
	};

	void evaluate() override {
		evals["error"] = (val>targetval ? val-targetval : targetval-val);
		evals["aux"] = 1.;
	};
};
