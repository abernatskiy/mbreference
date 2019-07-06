#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iterator>

#include "individual.h"
#include "../utils/strutil.h"

// Auxiliary functions

std::string evaluationsToCompactString(std::map<std::string,double> evals) {
	if(evals.size()==0)
		return "noevals";
	else {
		std::stringstream s;
		for(auto e : evals)
			s << e.first << ":" << e.second << ",";
		std::string out = s.str();
		out.pop_back();
		return out;
	}
}

std::map<std::string,double> compactStringToEvaluations(std::string str) {
	std::map<std::string,double> evals;

	std::istringstream sstream(str);
	for(auto csit=std::istream_iterator<WordDelimitedByComma>(sstream); csit!=std::istream_iterator<WordDelimitedByComma>(); csit++) {
		std::istringstream evalSStream(*csit);
		auto eit = std::istream_iterator<WordDelimitedByColon>(evalSStream);
		std::string paramName = *eit;
		eit++;
		double paramVal = std::stod(*eit);
		evals[paramName] = paramVal;
	}

	return evals;
}

// Individual class definitions and company

static long unsigned currentid = 0;

Individual::Individual() :
	id(currentid) {
	currentid++;
}

void Individual::renewId() {
	id = currentid;
	currentid++;
}

bool Individual::isAllEquivalentTo(const Individual& other, const std::vector<std::string>& evalNames) const {
	for(auto en : evalNames)
		if(evals.at(en) != other.evals.at(en))
			return false;
	return true;
}

bool Individual::isAnyBetterThan(const Individual& other, const std::vector<std::string>& evalNames) const {
	for(auto en : evalNames)
		if(evals.at(en) < other.evals.at(en))
			return true;
	return false;
}

bool Individual::isInNoWayWorseThan(const Individual& other, const std::vector<std::string>& evalNames) const {
	for(auto en : evalNames)
		if(evals.at(en) > other.evals.at(en))
			return false;
	return true;
}

bool Individual::isDominatedBy(const Individual& other, const std::vector<std::string>& evalNames) const {
//	D( if(id==other.id) { std::cerr << "Warning: individuals with the same id are checked for dominance!" << std::endl; } )

//	std::cout << "not worse: " << (isInNoWayWorseThan(other, evalNames)?"y":"n")
//	          << " any better: " << (isAnyBetterThan(other, evalNames)?"y":"n")
//	          << " all equivalent: " << (isAllEquivalentTo(other, evalNames)?"y":"n")
//	          << " id tie: " << (id < other.id?"y":"n") << std::endl;

	if(isAnyBetterThan(other, evalNames))
		return false;
	else if(BREAK_TIES_BY_ID && isAllEquivalentTo(other, evalNames))
		return id < other.id;
	else
		return true;
}

void Individual::loadStr(std::string str) {

	std::istringstream sstream(str);
	auto wit=std::istream_iterator<WordDelimitedBySpace>(sstream);

	id = std::stoul(*wit);
	if(currentid <= id)
		currentid = id + 1;
	wit++;

	evals = compactStringToEvaluations(*wit);
	wit++;

	loadGenomeStr(*wit);
	wit++;

	if(wit!=std::istream_iterator<WordDelimitedBySpace>()) {
		std::cerr << "Too many words in the individual desctiption! Exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
}
