#ifndef LEVI_CIVITA_H
#define LEVI_CIVITA_H

#include <iostream>
#include <vector>
#include <string>

using std::cout;
using std::cin;
using std::endl;

class LeviCivita {
public:
	LeviCivita() = default;
	LeviCivita(bool inpIsUpper);

	void print() const;
	void printLatex() const;


	//For debugging
	void inputByUser();

	void addIndex(const std::string& inpIndex);

	std::string getNameAtLocation(int location) const;
	int getLocationOfIndex(std::string indexName) const;

	int getSize() const {
		return indices.size();
	}

	//Assumes left one exists
	void swapIndexAtLocationWithOneOnLeft(int location);

	bool doesIndexExist(const std::string& index) const;

	void changeIndexAtLocationTo(int location, const std::string& newName);

	void swapIndexAtLocationWithOneOnRight(int location);

	bool operator==(const LeviCivita& rhs) const;
	bool operator!=(const LeviCivita& rhs) const;

	int numberOfSimilarIndices(const LeviCivita& otherLevi) const;

private:
	std::vector<std::string> indices;
	bool isUpper;
};

#endif

