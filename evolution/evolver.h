#pragma once

#include <random>
#include <fstream>
#include <limits>
#include <cstdlib>

#include "../constants.h"

#include "individual.h"

#ifndef EVOLVER_MAX_GENERATIONS
#define EVOLVER_MAX_GENERATIONS 10
#endif // EVOLVER_MAX_GENERATIONS

#ifndef EVOLVER_POPULATION_SIZE
#define EVOLVER_POPULATION_SIZE 10
#endif // EVOLVER_POPULATION_SIZE

#ifndef EVOLVER_RECORDS_PERIOD
#define EVOLVER_RECORDS_PERIOD 1
#endif // EVOLVER_RECORDS_PERIOD

#ifndef EVOLVER_EVALUATIONS_OF_INTEREST
#define EVOLVER_EVALUATIONS_OF_INTEREST {"error"}
#endif // EVOLVER_EVALUATIONS_OF_INTEREST

#ifndef EVOLVER_EVALUATIONS_OF_INTEREST_PERIOD
#define EVOLVER_EVALUATIONS_OF_INTEREST_PERIOD 1
#endif // EVOLVER_EVALUATIONS_OF_INTEREST_PERIOD

#ifndef EVOLVER_EVALUATIONS_OF_INTEREST_INDEPENDENT // set this to true if you want to find out the minimum of *each* eval across the population independently of other evals
                                                    // the default is to find the "best" individual with minimal first eval and use this individual for the rest of evals
#define EVOLVER_EVALUATIONS_OF_INTEREST_INDEPENDENT false
#endif // EVOLVER_EVALUATIONS_OF_INTEREST_INDEPENDENT

template<class MyIndividual>
class Evolver {
public:
	std::mt19937_64 rng;
	long unsigned curgen;
	std::vector<MyIndividual> population;
	std::string initialPopulationFilename;

	Evolver(unsigned randomSeed, std::string initPopFN) : rng(randomSeed), initialPopulationFilename(initPopFN) {};
	virtual ~Evolver() = default;

	void evolve() {
		if(!initialPopulationFilename.empty())
			loadInitialPopulationFromFile(initialPopulationFilename);
		else
			generateInitialPopulationRandomly();

		evaluatePopulation();

		curgen = 0;
		makeRecords();
		while(true) {
			curgen++;
			if(EVOLVER_MAX_GENERATIONS>=0 && curgen>EVOLVER_MAX_GENERATIONS)
				break;
			advanceForOneGeneration();
			evaluatePopulation();
			makeRecords();
		};
	};

	virtual void makeRecords() {
		// dump the population into a text file every once in a while
		if(EVOLVER_RECORDS_PERIOD>0 && curgen%EVOLVER_RECORDS_PERIOD==0) {
			std::string ofname = "generation" + std::to_string(curgen) + ".ssv";
			std::ofstream ofs(ofname, std::ofstream::out);
			for(const MyIndividual& indiv : population)
				ofs << indiv.str() << std::endl;
			ofs.close();
		}

		// print and save to a file evaluations of interest
		static const std::string championsFileName = "champions.ssv";
		static const std::vector<std::string> interestingEvals = EVOLVER_EVALUATIONS_OF_INTEREST;
		static std::map<std::string,double> interestingVals;

		static const MyIndividual* bestIndivPtr = nullptr;

		if(EVOLVER_EVALUATIONS_OF_INTEREST_PERIOD>0 && curgen%EVOLVER_EVALUATIONS_OF_INTEREST_PERIOD==0) {
			if(curgen==0) {
				std::ofstream ofs(championsFileName, std::ofstream::out);
				ofs.close();
			}

			interestingVals.clear();

			for(const auto& en : interestingEvals)
				interestingVals[en] = population[0].evals.at(en);
			if(!EVOLVER_EVALUATIONS_OF_INTEREST_INDEPENDENT)
				bestIndivPtr = &(population[0]);

			for(const auto& indiv : population) {
				if(EVOLVER_EVALUATIONS_OF_INTEREST_INDEPENDENT) {
					for(const auto& en : interestingEvals)
						if(indiv.evals.at(en)<interestingVals[en])
							interestingVals[en] = indiv.evals.at(en);
				}
				else {
					if(indiv.evals.at(interestingEvals[0])<interestingVals[interestingEvals[0]]) {
						bestIndivPtr = &indiv;
						for(const auto& en : interestingEvals)
							interestingVals[en] = indiv.evals.at(en);
					}
				}
			}
			std::cout << "gen " << curgen; // to the command line
			for(const auto& en : interestingEvals)
				std::cout << " " <<  en << ":" << interestingVals[en];
			std::cout << std::endl;

			std::ofstream ofs(championsFileName, std::ofstream::out | std::ofstream::app);
			ofs << curgen; // to the file
			for(const auto& en : interestingEvals)
				ofs << " " <<  en << ":" << interestingVals[en];
			if(bestIndivPtr)
				ofs << " " << bestIndivPtr->str();
			ofs << std::endl;
			ofs.close();

		}
	};

	virtual void generateInitialPopulationRandomly() { // the default is a fixed size population of random (whatever it means) individuals
		for(unsigned i=0; i<EVOLVER_POPULATION_SIZE; i++) {
			MyIndividual newindiv;
			newindiv.randomize(rng);
			population.push_back(newindiv);
		}
	};

	void loadInitialPopulationFromFile(std::string filename) {
		std::ifstream ipf(filename);
		std::string curline;
		if(ipf.is_open()) {
			while(std::getline(ipf, curline)) {
				MyIndividual newindiv;
				newindiv.loadStr(curline);
				population.push_back(newindiv);
			}
		} else {
			std::cerr << "Couldn't open initial population file " << filename << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	void evaluatePopulation() {
		#pragma omp parallel num_threads(NUM_THREADS)
		{
			#pragma omp for
			for(int i=0; i<EVOLVER_POPULATION_SIZE; i++) {
				population[i].evaluate();
			}
		}
	};

	virtual void advanceForOneGeneration() = 0; // no default method for selection!
};

template<class MyIndividual>
class TestEvolver : public Evolver<MyIndividual> { // simple hill climber (or rather, valley descender)
public:
	const std::string evalName;
	TestEvolver(std::string evaluationName, unsigned randomSeed)
		: Evolver<MyIndividual>(randomSeed, ""), evalName(evaluationName) {};
	void advanceForOneGeneration() override {
		double minEval = std::numeric_limits<double>::max();
		MyIndividual bestIndividual;
		for(const auto& indiv : this->population) {
			if(indiv.evals.at(evalName)<minEval) {
				minEval = indiv.evals.at(evalName);
				bestIndividual = indiv;
			}
		}
		this->population.clear();
		this->population.push_back(bestIndividual);
		while(this->population.size()<EVOLVER_POPULATION_SIZE) {
			MyIndividual newIndividual(bestIndividual);
			newIndividual.mutate(this->rng);
			this->population.push_back(newIndividual);
		}
	};
};
