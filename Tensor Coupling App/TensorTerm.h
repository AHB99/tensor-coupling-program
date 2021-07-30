#ifndef TENSOR_TERM_H
#define TENSOR_TERM_H

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

#include "Tensor.h"

using std::cout;
using std::cin;
using std::endl;

class TensorTerm {
public:
	void inputTensorTermByKeyboard();
	void printTensorTerm();
	void printTensorTermAsLatex();
	std::vector<std::string> getListOfNames();
	int findTensorLocationOfFirstOccurenceOfIndex(std::string indexName);
	void changeIndexNameTo(int tensorLocation, int indexLocation, std::string newName);
	void sortTensorTerm();

	Tensor getTensor(int location) {
		return tensors[location];
	}
	int getSize() const {
		return tensors.size();
	}

	void reorderIndicesOfTerm();
	//Assuming sorted term. If atleast 2 tensors cannot be ordered.
	bool isAmbiguous();

	//Assuming sorted term.
	void getAllAmbiguousZoneLocationsAndSizes(std::vector<std::pair<int, int>>& ambiguousZones);

	//Assuming sorted term.
	void getAllPermutationsOfAllAmbiguousZones(std::vector<TensorTerm>& allTensorTermPermutations);


	void addAllTensors(std::vector<Tensor>& inputTensors) {
		tensors = inputTensors;
	}
	void setCoefficient(int inpCoeff) {
		coefficient = inpCoeff;
	}

	int getCoefficient() const {
		return coefficient;
	}

	void addTermCoefficientIntoThisTerm(const TensorTerm& otherTerm);

	void addTensor(const Tensor& inputTensor) {
		tensors.push_back(inputTensor);
	}

	//If binaryMarker = 0, first occurence is unbarred, second is barred. Vice versa for binaryMarker = 1.
	void fillBarsBasedOnBinaryMarker(std::string indexName, int binaryMarker);

private:
	void getAllPermutationsOfLocationsForAllAmbiguousZones(std::vector<std::vector<std::vector<int>>>& locationPermutationsOfAllZones, std::vector<std::pair<int, int>>& ambiguousZones);



	std::vector<Tensor> tensors;
	int coefficient = 1;
};

#endif
