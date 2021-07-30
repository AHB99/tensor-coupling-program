#ifndef IRREDUCIBLE_TENSOR_H
#define IRREDUCIBLE_TENSOR_H

#include <iostream>
#include <vector>
#include <string>

using std::cout;
using std::cin;
using std::endl;

class IrreducibleTensor {
public:
	IrreducibleTensor() : isSymmetric(false), field(0), isBarred(false) { }
	IrreducibleTensor(bool inpIsSymmetric, int inpField);

	void print() const;
	void printLatex() const;


	//For debugging
	void inputByUser();

	bool doesIndexExist(std::string index) const;
	bool doesIndexExistInUpperZone(std::string index) const;
	bool doesIndexExistInLowerZone(std::string index) const;


	//True if present
	bool replaceIndexIfPresent(std::string oldIndex, std::string newIndex);

	void addUpperIndex(const std::string& index);
	void addLowerIndex(const std::string& index);

	std::string getUpperIndexAt(int location) const;
	std::string getLowerIndexAt(int location) const;

	int getNumberOfUpperIndices() const {
		return upperIndices.size();
	}
	int getNumberOfLowerIndices() const {
		return lowerIndices.size();
	}
	bool getSymmetryState() const {
		return isSymmetric;
	}
	bool getBarState() const {
		return isBarred;
	}
	int getField() const {
		return field;
	}


	void setBarred(bool inpBarred) {
		isBarred = inpBarred;
	}

	void swapUpperIndexAtLocationWithOneOnRight(int location);
	void swapLowerIndexAtLocationWithOneOnRight(int location);

	void changeUpperIndexAtLocationTo(int location, const std::string& newName);
	void changeLowerIndexAtLocationTo(int location, const std::string& newName);

	bool operator==(const IrreducibleTensor& rhs) const;
	bool operator!=(const IrreducibleTensor& rhs) const;

	bool isSameStructure(const IrreducibleTensor& otherIrreducibleTensor) const;

	bool hasAtleast2MatchingIndices(const IrreducibleTensor& otherIrreducibleTensor) const;

	//Returns actual SO(10) field from program's encoded version
	std::string getPhysicalFieldFromEncodedField() const;

private:
	std::vector<std::string> upperIndices;
	std::vector<std::string> lowerIndices;
	bool isSymmetric;
	//number equivalent to number of indices of the field
	int field;
	bool isBarred = false;
};

#endif
