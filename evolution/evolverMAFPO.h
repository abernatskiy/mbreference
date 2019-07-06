// Age-Fitness Pareto Optimization algorithm variant that allows more than one fitness objective

#pragma once

#include <vector>
#include <string>
#include <fstream>

#include "evolver.h"

#ifndef EVOLVER_MAFPO_RECORDS_PERIOD
#define EVOLVER_MAFPO_RECORDS_PERIOD 1
#endif // EVOLVER_MAFPO_RECORDS_PERIOD

inline std::vector<std::string> appendAge(std::vector<std::string> objlist) { objlist.push_back("age"); return objlist; };

template<class MyIndividual>
class MAFPOEvolver : public Evolver<MyIndividual> {
private:
	const std::vector<std::string> objectives; // multiple objectives of the algorithm, including age
	std::vector<MyIndividual> paretoFront;

public:
	MAFPOEvolver(unsigned randSeed, std::string initialPopulationFilename, std::vector<std::string> objs) :
		Evolver<MyIndividual>(randSeed, initialPopulationFilename), objectives(appendAge(objs)), paretoFront() {}; // objs must not include "age" or "isDominated"
	void makeRecords() override; // we need additional records about the Pareto front size
	void generateInitialPopulationRandomly() override; // age must be initialized in addition to the default procedure
	void advanceForOneGeneration() override;
};

// class definitions

template<class MyIndividual>
void MAFPOEvolver<MyIndividual>::makeRecords() {
	Evolver<MyIndividual>::makeRecords(); // standard records: population dumps

	// additional record about the Pareto front
	static const std::string ofname = "paretoFronts.ssv";
	if(EVOLVER_MAFPO_RECORDS_PERIOD>0 && this->curgen>0 && (this->curgen-1)%EVOLVER_MAFPO_RECORDS_PERIOD==0) {
		if(this->curgen-1==0) {
			std::ofstream ofs(ofname, std::ofstream::out);
			ofs.close();
		}

		std::ofstream ofs(ofname, std::ofstream::out | std::ofstream::app);
		ofs << this->curgen-1;
		for(const auto& indiv : paretoFront)
			ofs << " " << indiv.id;
		ofs << std::endl;
		ofs.close();
	}
}

template<class MyIndividual>
void MAFPOEvolver<MyIndividual>::generateInitialPopulationRandomly() {
	Evolver<MyIndividual>::generateInitialPopulationRandomly();
	for(auto& indiv : this->population)
		indiv.evals["age"] = 0.;
}

template<class MyIndividual>
void MAFPOEvolver<MyIndividual>::advanceForOneGeneration() {
	// Marking all the individuals that are dominated by at least one other
	for(auto& first : this->population) {
		first.evals["isDominated"] = 0.;
		for(const auto& second : this->population) {
			if(first.isDominatedBy(second, objectives)) {
				first.evals["isDominated"] = 1.;
				break;
			}
		}
	}

	// The ones that remain standing form the Pareto front
	paretoFront.clear();
	for(const auto& indiv : this->population) {
		if(indiv.evals.at("isDominated") == 0.)
			paretoFront.emplace_back(indiv);
	}

	// They form the beginning of the new population
	this->population.clear();
	for(const auto& indiv : paretoFront)
		this->population.emplace_back(indiv);

	// New individuals of the population, except one, are offspring of the Pareto front individuals
	std::uniform_int_distribution<unsigned> parentPicker(0, paretoFront.size()-1);
	while(this->population.size()<EVOLVER_POPULATION_SIZE-1) {
		MyIndividual child(paretoFront[parentPicker(this->rng)]);
		while(!child.mutate(this->rng)) {}
		this->population.push_back(child);
	}

	// Increasing age of every individual that originate from ol lineages
	for(auto& indiv : this->population)
		indiv.evals["age"] += 1.;

	// Adding a new lineage, initially just one random individual
	MyIndividual newIndiv;
	newIndiv.randomize(this->rng);
	newIndiv.evals["age"] = 0.;
	this->population.push_back(newIndiv);
}
