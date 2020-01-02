#ifndef TENSOR_H
#define TENSOR_H

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>


using std::cout;
using std::cin;
using std::endl;

class BbtChain;

class Tensor {
public:
	void addIndex(std::string name, int barstatus);
	void setLabel(std::string inpLabel);
	int getNumberOfIndices() const;
	void printTensor() const;
	void printLatex() const;
	//Assuming bars left, unbars right.
	int getUnbarredZoneLocation();

	//Assuming bars left, unbars right.
	int getBarredZoneLocation();

	bool doesNameExist(std::string name);

	//Assuming bars left, unbars right.
	bool doesNameExistInUnbarredZone(std::string name);

	//Assuming bars left, unbars right.
	bool doesNameExistInBarredZone(std::string name);

	//Assuming it exists
	void changeIndexNameTo(int location, std::string newName);

	void swapIndexWithOneOnRight(int location);
	void swapIndexWithOneOnLeft(int location);


	std::string getIndexNameAt(int location) {
		return indices[location].first;
	}

	int getBarStateAt(int location) {
		return indices[location].second;
	}

	void setBarStateAt(int barState, int location) {
		indices[location].second = barState;
	}

	std::string getLabel() const {
		return label;
	}

	int getBarStateFromName(std::string name);

	int getNumberOfBarredIndices() const;

	//Phase 2
	bool replaceIndexIfPresent(const std::string& oldIndex, const std::string& newIndex);
	BbtChain convertToBbtChain();
	int getNumberOfDuplicatedIndices() const;
	std::vector<std::string> getDuplicatedIndices() const;
	int getLocationOfFirstOccurenceOfIndex(const std::string& indexName) const;
	int getLocationOfSecondOccurenceOfIndex(const std::string& indexName) const;

	std::vector<std::string> getIndices() const {
		std::vector<std::string> result;
		for (auto& index : indices) {
			result.push_back(index.first);
		}
		return result;
	}

private:
	//vector of indices. Index: name, barstatus.
	//Bar status: 0 = unbarred. 1 = barred.
	std::vector<std::pair<std::string, int>> indices;
	std::string label;
};

#endif