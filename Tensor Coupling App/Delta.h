#ifndef DELTA_H
#define DELTA_H

#include <iostream>
#include <vector>
#include <string>


using std::cout;
using std::cin;
using std::endl;

class IrreducibleTensor;
class LeviCivita;
class MatterTensor;
class Tensor;

class Delta {
public:
	Delta() = default;
	Delta(const std::string& inpUpperIndex, const std::string& inpLowerIndex);

	void print() const;
	void printLatex() const;


	//For debugging
	void inputByUser();

	//Only if exactly 1 index can be summed
	bool sumIndicesIfPossible(const Delta& otherDelta);
	
	bool isCancellationDeltaForIrreducibleTensor(const IrreducibleTensor& tensor) const;

	//True if possible
	bool replaceIndexIfPossible(IrreducibleTensor& tensor) const;

	//Assumes each delta has different indices
	bool isIdentical(const Delta& otherDelta) const;

	void setIndices(const std::string& inpUpperIndex, const std::string& inpLowerIndex);
	void setUpperIndex(const std::string& inpUpperIndex);
	void setLowerIndex(const std::string& inpLowerIndex);



	std::string getUpperIndex() const {
		return upperIndex;
	}
	std::string getLowerIndex() const {
		return lowerIndex;
	}

	//Phase 2
	bool replaceIndexIfPossible(Tensor& tensor) const;
	bool replaceIndexIfPossible(MatterTensor& tensor) const;
	bool replaceIndexIfPossible(LeviCivita& tensor) const;
	bool isCancellationDeltaForMatterTensor(const MatterTensor& tensor) const;


private:
	std::string upperIndex;
	std::string lowerIndex;
};

#endif