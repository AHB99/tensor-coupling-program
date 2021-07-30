#ifndef NORMALIZED_TENSOR_H
#define NORMALIZED_TENSOR_H

#include <iostream>
#include <vector>
#include <string>

using std::cout;
using std::cin;
using std::endl;

class IrreducibleTensor;

class NormalizedTensor {
public:
	NormalizedTensor() : isSymmetric(false), field(0), isBarred(false) { }
	NormalizedTensor(bool inpIsSymmetric, int inpField);

	void print() const;
	void printLatex() const;

	void addUpperIndex(const std::string& index);
	void addLowerIndex(const std::string& index);
	void setBarred(bool inpBarred) {
		isBarred = inpBarred;
	}

	void setIndicesFromIrreducibleTensor(const IrreducibleTensor& irreducibleTensor);

	//Returns actual SO(10) field from program's encoded version
	std::string getPhysicalFieldFromEncodedField() const;

private:
	std::vector<std::string> upperIndices;
	std::vector<std::string> lowerIndices;
	bool isSymmetric;
	int field;
	bool isBarred = false;
};

#endif
