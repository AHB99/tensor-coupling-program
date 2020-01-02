#include "TensorTerm.h"
#include "MathExpressionTerm.h"
#include "UtilityFunctions.h"

// TENSOR TERM MEMBER DEFS

void TensorTerm::inputTensorTermByKeyboard() {
	cout << "Enter coefficient:";
	coefficient.inputByUser();
	int numOfTensors;
	cout << "Enter number of tensors: ";
	cin >> numOfTensors;
	for (int i = 0; i < numOfTensors; ++i) {
		cout << "Tensor " << i << ": " << endl;

		Tensor tempTensor;
		int numOfIndices;
		std::string tempTensorLabel;

		cout << "Enter tensor label: ";
		cin >> tempTensorLabel;
		tempTensor.setLabel(tempTensorLabel);

		cout << "Enter number of indices: ";
		cin >> numOfIndices;
		for (int j = 0; j < numOfIndices; ++j) {
			cout << "Index " << j << ": " << endl;
			std::string tempIndexName;
			int tempBarState;

			cout << "Enter name of index and barstate (0 = unbarred, 1 = barred): ";
			cin >> tempIndexName;
			cin >> tempBarState;

			tempTensor.addIndex(tempIndexName, tempBarState);
		}
		tensors.push_back(tempTensor);
	}
}

void TensorTerm::printTensorTerm() {
	cout << coefficient << "*";
	for (int i = 0; i < tensors.size(); ++i) {
		tensors[i].printTensor();
	}
	cout << endl;
}



void TensorTerm::printTensorTermAsLatex() {
	if (coefficient > 0) {
		cout << "+";
		if (coefficient > 1) {
			cout << coefficient;
		}
	}
	else if (coefficient == -1) {
		cout << "-";
	}
	//Because a negative number will already print it's negative sign
	else if (coefficient < -1) {
		cout << coefficient;
	}
	for (auto& tensor : tensors) {
		tensor.printLatex();
	}

}


std::vector<std::string> TensorTerm::getListOfNames() {
	std::vector<std::string> result;
	for (auto& tensor : tensors) {
		for (int i = 0; i < tensor.getNumberOfIndices(); ++i) {
			std::string currentName = tensor.getIndexNameAt(i);
			if ((result.empty()) || (std::find(result.begin(), result.end(), currentName) == result.end())) {
				result.push_back(currentName);
			}
		}

	}
	return result;
}

int TensorTerm::findTensorLocationOfFirstOccurenceOfIndex(std::string indexName) {
	for (int i = 0; i < tensors.size(); ++i) {
		if (tensors[i].doesNameExist(indexName)) {
			return i;
		}
	}
	return -1;
}

void TensorTerm::changeIndexNameTo(int tensorLocation, int indexLocation, std::string newName) {
	tensors[tensorLocation].changeIndexNameTo(indexLocation, newName);
}

void TensorTerm::reorderIndicesOfTerm() {
	for (auto& tensor : tensors) {
		bool modified = true;
		while (modified) {
			modified = false;
			for (int i = 0; i < tensor.getNumberOfIndices() - 1; ++i) {
				if (tensor.getBarStateAt(i) == tensor.getBarStateAt(i + 1)) {
					//If they are the same barstate and the current value is less than the next.
					if ((tensor.getIndexNameAt(i)>tensor.getIndexNameAt(i + 1))) {
						tensor.swapIndexWithOneOnRight(i);
						coefficient *= -1;
						modified = true;
					}
				}
				else {
					if ((tensor.getBarStateAt(i) == 0) && (tensor.getBarStateAt(i + 1) == 1)) {
						tensor.swapIndexWithOneOnRight(i);
						coefficient *= -1;
						modified = true;
					}
				}
			}
		}
	}
}

void TensorTerm::sortTensorTerm() {
	std::stable_sort(tensors.begin(), tensors.end(), compareTensorsByNumberOfBarredIndices);
	std::stable_sort(tensors.begin(), tensors.end(), compareTensorsByNumberOfIndices);
	std::stable_sort(tensors.begin(), tensors.end(), compareTensorsByLabel);
}

bool TensorTerm::isAmbiguous() {
	for (int i = 0; i < tensors.size() - 1; ++i) {
		if ((tensors[i].getLabel() == tensors[i + 1].getLabel()) && (tensors[i].getNumberOfIndices() == tensors[i + 1].getNumberOfIndices()) && (tensors[i].getNumberOfBarredIndices() == tensors[i + 1].getNumberOfBarredIndices())) {
			return true;
		}
	}
	return false;
}

void TensorTerm::getAllAmbiguousZoneLocationsAndSizes(std::vector<std::pair<int, int>>& ambiguousZones) {
	ambiguousZones.clear();
	int currentZoneLocation = 0;
	int currentZoneSize = 1;
	for (int i = 0; i < tensors.size() - 1; ++i) {
		if ((tensors[i].getLabel() == tensors[i + 1].getLabel()) && (tensors[i].getNumberOfIndices() == tensors[i + 1].getNumberOfIndices()) && (tensors[i].getNumberOfBarredIndices() == tensors[i + 1].getNumberOfBarredIndices())) {
			//If it's the first element of this zone
			if (currentZoneSize == 1) {
				currentZoneLocation = i;
			}
			++currentZoneSize;
		}
		else {
			//If we broke a streak
			if (currentZoneSize != 1) {
				std::pair<int, int> newZone(currentZoneLocation, currentZoneSize);
				ambiguousZones.push_back(newZone);
				currentZoneSize = 1;
			}
		}
	}
	if (currentZoneSize != 1) {
		std::pair<int, int> newZone(currentZoneLocation, currentZoneSize);
		ambiguousZones.push_back(newZone);
	}
}

void TensorTerm::getAllPermutationsOfLocationsForAllAmbiguousZones(std::vector<std::vector<std::vector<int>>>& locationPermutationsOfAllZones, std::vector<std::pair<int, int>>& ambiguousZones) {
	//For every ambiguous zone
	for (auto& ambiguousZone : ambiguousZones) {
		std::vector<std::vector<int>> allPermutationsOfLocationsOfCurrentZone;
		std::vector<int> originalArrangement;
		for (int i = 0; i < ambiguousZone.second; ++i) {
			originalArrangement.push_back(i);
		}

		for (int i = 0; i < factorial(ambiguousZone.second); ++i) {
			std::next_permutation(originalArrangement.begin(), originalArrangement.end());
			allPermutationsOfLocationsOfCurrentZone.push_back(originalArrangement);
		}
		locationPermutationsOfAllZones.push_back(allPermutationsOfLocationsOfCurrentZone);
	}
}

void TensorTerm::getAllPermutationsOfAllAmbiguousZones(std::vector<TensorTerm>& allTensorTermPermutations) {
	std::vector<std::pair<int, int>> ambiguousZones;
	getAllAmbiguousZoneLocationsAndSizes(ambiguousZones);

	std::vector<std::vector<std::vector<int>>> locationPermutationsOfAllZones;
	getAllPermutationsOfLocationsForAllAmbiguousZones(locationPermutationsOfAllZones, ambiguousZones);

	std::vector<int> locationCounters(locationPermutationsOfAllZones.size(), 0);

	long int maxNumberOfCombos = 1;
	for (auto& list : locationPermutationsOfAllZones) {
		maxNumberOfCombos *= list.size();
	}

	for (long int i = 0; i < maxNumberOfCombos; ++i) {
		std::vector<Tensor> currentArrangementOfTensors(tensors);
		//Add the tensors to it from the counter. For each list of permutations:
		for (int j = 0; j < locationPermutationsOfAllZones.size(); ++j) {
			//Replace the Tensors with the permuted version
			std::vector<Tensor> permutationToInsert;
			for (int k = 0; k < locationPermutationsOfAllZones[j][0].size(); ++k) {
				int tensorLocation = locationPermutationsOfAllZones[j][locationCounters[j]][k] + ambiguousZones[j].first;
				permutationToInsert.push_back(tensors[tensorLocation]);
			}
			//Reflect the current zone in the current Arrangement
			std::copy(permutationToInsert.begin(), permutationToInsert.end(), currentArrangementOfTensors.begin() + ambiguousZones[j].first);
		}
		TensorTerm tempTerm;
		tempTerm.addAllTensors(currentArrangementOfTensors);
		tempTerm.setCoefficient(coefficient);
		allTensorTermPermutations.push_back(tempTerm);

		//Increment the first zone's counter
		locationCounters[0]++;
		//Update counter
		for (int j = 0; j < locationCounters.size() - 1; ++j) {
//			if (locationCounters[j] >= ambiguousZones[j].second) {


			//If a counter exceeds it's zone's number of permutations, increment the next zone index and reset current to 0.
			if (locationCounters[j] >= locationPermutationsOfAllZones[j].size()) {
				locationCounters[j + 1]++;
				locationCounters[j] = 0;
			}
		}


	}


}

void TensorTerm::fillBarsBasedOnBinaryMarker(std::string indexName, int binaryMarker) {
	bool encountered = false;
	for (auto& tensor : tensors) {
		//For every index in every tensor
		for (int i = 0; i < tensor.getNumberOfIndices(); ++i) {
			//If we find a match with the target name
			if (tensor.getIndexNameAt(i) == indexName) {
				//First encounter, binaryMarker = 0 makes it unbarred, = 1 makes it barred.
				if (!encountered) {
					if (binaryMarker == 0) {
						tensor.setBarStateAt(0, i);
					}
					else if (binaryMarker == 1) {
						tensor.setBarStateAt(1, i);
					}
					//Record the encounter
					encountered = true;
				}
				//Second encounter, binaryMarker = 0 makes it barred, = 1 makes it unbarred.
				else {
					if (binaryMarker == 0) {
						tensor.setBarStateAt(1, i);
					}
					else if (binaryMarker == 1) {
						tensor.setBarStateAt(0, i);
					}
				}
			}
		}
	}
}

void TensorTerm::addTermCoefficientIntoThisTerm(const TensorTerm& otherTerm) {
	coefficient += otherTerm.getCoefficient();
}


//***************************PHASE 2*********************
MathExpressionTerm TensorTerm::convertToMet(const MathExpressionTerm& templateMet) {
	MathExpressionTerm resultMet(templateMet);
	std::vector<Tensor> tempReducibleTensors;
	for (auto& tensor : tensors) {
		if (tensor.getLabel() == "GAMMA") {
			resultMet.setRightBbtChain(tensor.convertToBbtChain());
		}
		else {
			tempReducibleTensors.push_back(tensor);
		}
	}
	resultMet.setReducibleTensors(tempReducibleTensors);
	resultMet.setCoefficient(coefficient);

	return resultMet;
}

void TensorTerm::shiftDuplicatedIndexToLeft(const std::string& duplicatedIndex, int tensorLocation) {
	int currentLocation = tensors[tensorLocation].getLocationOfFirstOccurenceOfIndex(duplicatedIndex);
	while (currentLocation > 0) {
		tensors[tensorLocation].swapIndexWithOneOnLeft(currentLocation);
		coefficient *= -1;
		--currentLocation;
	}

	currentLocation = tensors[tensorLocation].getLocationOfSecondOccurenceOfIndex(duplicatedIndex);
	int boundary;
	//If unbarred, go till second spot, else till first spot
	if (tensors[tensorLocation].getBarStateAt(currentLocation) == 0) {
		boundary = 1;
	}
	else {
		boundary = 0;
	}
	while (currentLocation > boundary) {
		tensors[tensorLocation].swapIndexWithOneOnLeft(currentLocation);
		coefficient *= -1;
		--currentLocation;
	}
}


