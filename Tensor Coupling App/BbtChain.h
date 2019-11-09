#pragma once

#include "Bbt.h"
#include "Tensor.h"
#include <vector>
#include <iostream>

using std::cout;
using std::cin;
using std::endl;

//Example form: bt bt b b b bt bt bt bt
struct BbtChainCounter {
	int bCount = 0, btAfterBCount = 0, btTotalCount = 0;
};

class BbtChain {
public:
	void print() const;
	void inputByUser();

	//To reuse existing simplification code
	Tensor convertToReducibleTensorEquivalent() const;

	void addBbt(std::string inpName, bool inpDagger) {
		Bbt temp(inpName, inpDagger);
		bbtChain.push_back(temp);
	}
	int getSize() const {
		return bbtChain.size();
	}
	std::string getBbtNameAt(int location) const {
		return bbtChain[location].getName();
	}
	bool isDaggerAt(int location) const {
		return bbtChain[location].isDagger();
	}
	void setBbtDaggerStateAt(bool inpDagger, int location) {
		bbtChain[location].setDagger(inpDagger);
	}
	void swapBbtAtLocationWithOneOnRight(int location) {
		if (location < bbtChain.size()-1) {
			std::swap(bbtChain[location], bbtChain[location + 1]);
		}
	}

	void removeBbtAtLocation(int location) {
		bbtChain.erase(bbtChain.begin() + location);
	}

	std::vector<std::string> getIndices() const {
		std::vector<std::string> result;
		for (auto& bbt : bbtChain) {
			result.push_back(bbt.getName());
		}
		return result;
	}

	BbtChainCounter getBbtChainCounter() const;

	void deleteBbtByRange(int first, int last);

	void clear() {
		bbtChain.clear();
	}

private:
	std::vector<Bbt> bbtChain;

};
