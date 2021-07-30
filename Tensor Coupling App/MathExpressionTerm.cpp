#include "MathExpressionTerm.h"
#include "MathExpression.h"
#include "UtilityFunctions.h"
#include <algorithm>

void MathExpressionTerm::print() const {
	cout << coefficient << "*";
	for (auto beg = deltas.begin(); beg < deltas.end(); ++beg) {
		beg->print();
		cout << "*";
	}
	for (auto beg = leviCevitas.begin(); beg < leviCevitas.end(); ++beg) {
		beg->print();
		cout << "*";
	}
	for (auto beg = irreducibleTensors.begin(); beg < irreducibleTensors.end(); ++beg) {
		beg->print();
		if (beg != irreducibleTensors.end() - 1) {
			cout << "*";
		}
	}
	for (auto beg = normalizedTensors.begin(); beg < normalizedTensors.end(); ++beg) {
		beg->print();
		if (beg != normalizedTensors.end() - 1) {
			cout << "*";
		}
	}
}

void MathExpressionTerm::printLatex() const {
	coefficient.printLatex();
	for (auto beg = deltas.begin(); beg < deltas.end(); ++beg) {
		beg->printLatex();
	}
	for (auto beg = leviCevitas.begin(); beg < leviCevitas.end(); ++beg) {
		beg->printLatex();
	}
	for (auto beg = irreducibleTensors.begin(); beg < irreducibleTensors.end(); ++beg) {
		beg->printLatex();
	}
	for (auto beg = normalizedTensors.begin(); beg < normalizedTensors.end(); ++beg) {
		beg->printLatex();
	}
}


void MathExpressionTerm::inputByUser() {
	int tempNum;
	cout << "Number of Deltas: ";
	cin >> tempNum;
	for (int i = 0; i < tempNum; ++i) {
		Delta tempDelta;
		tempDelta.inputByUser();
		deltas.push_back(tempDelta);
	}
	cout << "Number of LeviCivitas: ";
	cin >> tempNum;
	for (int i = 0; i < tempNum; ++i) {
		LeviCivita tempLevi;
		tempLevi.inputByUser();
		leviCevitas.push_back(tempLevi);
	}
	cout << "Number of Irreducible Tensors: ";
	cin >> tempNum;
	for (int i = 0; i < tempNum; ++i) {
		IrreducibleTensor tempIrr;
		tempIrr.inputByUser();
		irreducibleTensors.push_back(tempIrr);
	}
	cout << "Coefficient: ";
	coefficient.inputByUser();
}

void MathExpressionTerm::simplifyCoefficient() {
	coefficient.simplify();
}


void MathExpressionTerm::mergeTerms(const MathExpressionTerm& otherTerm) {
	coefficient.multiplyAssignWithoutSimplifying(otherTerm.coefficient);

	//Merge the deltas
	deltas.insert(deltas.end(), otherTerm.deltas.begin(), otherTerm.deltas.end());

	//Merge the levis
	leviCevitas.insert(leviCevitas.end(), otherTerm.leviCevitas.begin(), otherTerm.leviCevitas.end());

	//Merge the irreducible tensors
	irreducibleTensors.insert(irreducibleTensors.end(), otherTerm.irreducibleTensors.begin(), otherTerm.irreducibleTensors.end());


}

void MathExpressionTerm::sumOverDeltas() {
	bool changeMade = true;
	//Do not attempt if no deltas exist
	while (changeMade && (!deltas.empty())) {
		//For every delta, check every other delta
		for (int i = 0; i < deltas.size(); ++i) {
			for (int j = 0; j < deltas.size(); ++j) {
				if ((i != j) && (deltas[i].sumIndicesIfPossible(deltas[j]))) {
					deltas.erase(deltas.begin() + j);
					changeMade = true;
					break;
				}
				changeMade = false;
			}
			if (changeMade) {
				break;
			}
		}
	}
}

bool MathExpressionTerm::checkForCancellationDeltas() {
	for (int i = 0; i < deltas.size(); ++i) {
		for (int j = 0; j < irreducibleTensors.size(); ++j) {
			if (deltas[i].isCancellationDeltaForTensor(irreducibleTensors[j])) {
				coefficient = 0;
				return true;
			}
		}
	}
	return false;
}

void MathExpressionTerm::renameIrreducibleTensorsByDeltasIfPossible() {
	for (int i = 0; i < deltas.size(); ++i) {
		for (int j = 0; j < irreducibleTensors.size(); ++j) {
			//If a rename occured
			if (deltas[i].replaceIndexIfPossible(irreducibleTensors[j])) {
				//Delete that delta
				deltas.erase(deltas.begin() + i);
				//Decrement 'i' so it stays in the same position for next unchecked delta
				--i;
				break;
			}
		}
	}
}

void MathExpressionTerm::solveMatchingDeltas() {
	for (int i = 0; i < deltas.size(); ++i) {
		//For each delta, check every delta after it (previous ones should have been caught already)
		for (int j = i; j < deltas.size(); ++j) {
			//If deltas are identical (don't check self)
			if ((i != j) && (deltas[i].isIdentical(deltas[j]))) {
				//Erase both, the outer one first so validity of 'j' remains
				deltas.erase(deltas.begin() + j);
				deltas.erase(deltas.begin() + i);
				coefficient *= 5;
				//Decrement counter so it remains in same position next loop
				--i;
				break;
			}
		}
	}
}

bool MathExpressionTerm::simplifyTermByDeltas() {


	sumOverDeltas();
	if (checkForCancellationDeltas()) {
		return false;
	}
	renameIrreducibleTensorsByDeltasIfPossible();
	solveMatchingDeltas();
	return true;
}




void MathExpressionTerm::addIrreducibleTensor(const IrreducibleTensor& inpIrrTensor) {
	irreducibleTensors.push_back(inpIrrTensor);
}

void MathExpressionTerm::addDelta(const Delta& inpDelta) {
	deltas.push_back(inpDelta);
}

void MathExpressionTerm::addLeviCivita(const LeviCivita& inpLeviCivita) {
	leviCevitas.push_back(inpLeviCivita);
}

void MathExpressionTerm::setCoefficient(int numerator, int denominator) {
	coefficient = Coefficient(numerator, denominator);
}

void MathExpressionTerm::setCoefficient(int regularInteger) {
	coefficient = Coefficient(regularInteger);
}

void MathExpressionTerm::setCoefficient(Coefficient otherCoefficient) {
	coefficient = otherCoefficient;
}


void MathExpressionTerm::getNamesInCommonBetweenLevis(std::vector<std::string>& namesInCommon, int locationOfFirstLevi, int locationOfSecondLevi) const {
	for (int i = 0; i < leviCevitas[locationOfFirstLevi].getSize(); ++i) {
		for (int j = 0; j < leviCevitas[locationOfSecondLevi].getSize(); ++j) {
			if (leviCevitas[locationOfFirstLevi].getNameAtLocation(i) == leviCevitas[locationOfSecondLevi].getNameAtLocation(j)) {
				namesInCommon.push_back(leviCevitas[locationOfFirstLevi].getNameAtLocation(i));
				break;
			}
		}
	}
}

void MathExpressionTerm::moveNamesInCommonBetweenLevisToLeft(std::vector<std::string>& namesInCommon, int locationOfFirstLevi, int locationOfSecondLevi) {
	//Here, "i" also represents the location a particular name must go to in each Levi
	for (int i = 0; i < namesInCommon.size(); ++i) {
		//For first tensor
		int locationOfIndexInLevi = leviCevitas[locationOfFirstLevi].getLocationOfIndex(namesInCommon[i]);
		while (locationOfIndexInLevi != i) {
			leviCevitas[locationOfFirstLevi].swapIndexAtLocationWithOneOnLeft(locationOfIndexInLevi);
			coefficient *= -1;
			--locationOfIndexInLevi;
		}
		//For second tensor
		locationOfIndexInLevi = leviCevitas[locationOfSecondLevi].getLocationOfIndex(namesInCommon[i]);
		while (locationOfIndexInLevi != i) {
			leviCevitas[locationOfSecondLevi].swapIndexAtLocationWithOneOnLeft(locationOfIndexInLevi);
			coefficient *= -1;
			--locationOfIndexInLevi;
		}

	}
}

MathExpression MathExpressionTerm::expandPairOfLeviCivitas(const MathExpression& pred2Match, const MathExpression& pred1Match, const MathExpression& pred0Match) {

	std::vector<std::string > termCommonNames;
	//Choose the first pair of Levis
	getNamesInCommonBetweenLevis(termCommonNames, 0, 1);
	moveNamesInCommonBetweenLevisToLeft(termCommonNames, 0, 1);

	MathExpression substitutedLeviExpression;
	substitutedLeviExpression.setSubstitutionForLeviCivita(termCommonNames.size(), leviCevitas[0], leviCevitas[1], pred2Match, pred1Match, pred0Match);
	return substitutedLeviExpression;

}

void MathExpressionTerm::deletePairOfLeviCivitas() {
	leviCevitas.erase(leviCevitas.begin(), leviCevitas.begin() + 2);
}

void MathExpressionTerm::reorderIndicesOfIrreducibleTensorsAndLeviCivitas() {
	for (auto& tensor : irreducibleTensors) {
		//Reorder upper indices
		bool modified = true;
		while (modified) {
			modified = false;
			//Check till second last index
			for (int i = 0; i < tensor.getNumberOfUpperIndices() - 1; ++i) {
				if ((tensor.getUpperIndexAt(i) > tensor.getUpperIndexAt(i + 1))) {
					tensor.swapUpperIndexAtLocationWithOneOnRight(i);
					if (!tensor.getSymmetryState()) {
						coefficient *= -1;
					}
					modified = true;
				}

			}
		}
		//Reorder lower indices
		modified = true;
		while (modified) {
			modified = false;
			//Check till second last index
			for (int i = 0; i < tensor.getNumberOfLowerIndices() - 1; ++i) {
				if ((tensor.getLowerIndexAt(i) > tensor.getLowerIndexAt(i + 1))) {
					tensor.swapLowerIndexAtLocationWithOneOnRight(i);
					if (!tensor.getSymmetryState()) {
						coefficient *= -1;
					}
					modified = true;
				}

			}
		}
	}
	for (auto& levi : leviCevitas) {
		//Reorder indices
		bool modified = true;
		while (modified) {
			modified = false;
			//Check till second last index
			for (int i = 0; i < levi.getSize() - 1; ++i) {
				if ((levi.getNameAtLocation(i) > levi.getNameAtLocation(i + 1))) {
					levi.swapIndexAtLocationWithOneOnRight(i);
					coefficient *= -1;
					modified = true;
				}
			}
		}
	}
}

void MathExpressionTerm::sortIrreducibleTensors() {
	std::stable_sort(irreducibleTensors.begin(), irreducibleTensors.end(), compareIrreducibleTensorsByNumberOfUpperIndices);
	std::stable_sort(irreducibleTensors.begin(), irreducibleTensors.end(), compareIrreducibleTensorsByNumberOfLowerIndices);
	std::stable_sort(irreducibleTensors.begin(), irreducibleTensors.end(), compareIrreducibleTensorsByBarState);
	std::stable_sort(irreducibleTensors.begin(), irreducibleTensors.end(), compareIrreducibleTensorsBySymmetryState);
	std::stable_sort(irreducibleTensors.begin(), irreducibleTensors.end(), compareIrreducibleTensorsByField);
}

int MathExpressionTerm::getTensorLocationOfFirstOccurenceOfIndex(const std::string& index) const {
	//Check Levi first, as it is positioned on left always
	if (leviCevitas.size() == 1) {
		if (leviCevitas[0].doesIndexExist(index)) {
			return -1;
		}
	}
	for (int i = 0; i < irreducibleTensors.size(); ++i) {
		if (irreducibleTensors[i].doesIndexExist(index)) {
			return i;
		}
	}
	return -2;
}

std::vector<std::string> MathExpressionTerm::getListOfAllIndices() const {
	std::vector<std::string> result;
	for (auto& tensor : irreducibleTensors) {
		for (int i = 0; i < tensor.getNumberOfUpperIndices(); ++i) {
			std::string currentName = tensor.getUpperIndexAt(i);
			if ((result.empty()) || (std::find(result.begin(), result.end(), currentName) == result.end())) {
				result.push_back(currentName);
			}
		}
		for (int i = 0; i < tensor.getNumberOfLowerIndices(); ++i) {
			std::string currentName = tensor.getLowerIndexAt(i);
			if ((result.empty()) || (std::find(result.begin(), result.end(), currentName) == result.end())) {
				result.push_back(currentName);
			}
		}
	}
	//Only if single levi
	if (leviCevitas.size() == 1) {
		for (int i = 0; i < leviCevitas[0].getSize(); ++i) {
			std::string currentName = leviCevitas[0].getNameAtLocation(i);
			if ((result.empty()) || (std::find(result.begin(), result.end(), currentName) == result.end())) {
				result.push_back(currentName);
			}
		}
	}
	return result;
}

void MathExpressionTerm::performRenamesOnIrreducibleTensors(const std::vector<std::pair<std::string, std::string>> renameMap) {
	for (auto& tensor : irreducibleTensors) {
		//Change each upper index
		for (int i = 0; i < tensor.getNumberOfUpperIndices(); ++i) {
			tensor.changeUpperIndexAtLocationTo(i, getNewNameFromRenameMap(tensor.getUpperIndexAt(i), renameMap));
		}
		//Change each lower index
		for (int i = 0; i < tensor.getNumberOfLowerIndices(); ++i) {
			tensor.changeLowerIndexAtLocationTo(i, getNewNameFromRenameMap(tensor.getLowerIndexAt(i), renameMap));
		}
	}
}

void MathExpressionTerm::performRenamesOnLeviCivita(const std::vector<std::pair<std::string, std::string>> renameMap) {
	for (int i = 0; i < leviCevitas[0].getSize(); ++i) {
		leviCevitas[0].changeIndexAtLocationTo(i, getNewNameFromRenameMap(leviCevitas[0].getNameAtLocation(i), renameMap));
	}
}


void MathExpressionTerm::addMathExpressionTermCoefficientToThisTerm(const MathExpressionTerm& otherMET) {
	coefficient += otherMET.coefficient;
}

bool MathExpressionTerm::simplifyTermBySymmetricAsymmetricProperty() {


	for (auto& possiblySymmetricTensor : irreducibleTensors) {
		//For every symmetric Tensor
		if (possiblySymmetricTensor.getSymmetryState()) {
			//Check every asymmetric Tensor
			for (auto& possiblyAsymmetricTensor : irreducibleTensors) {
				if (!possiblyAsymmetricTensor.getSymmetryState()) {
					//If property is satisfied, cancel term
					if (possiblySymmetricTensor.hasAtleast2MatchingIndices(possiblyAsymmetricTensor)) {
						coefficient = 0;
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool MathExpressionTerm::isAmbiguous() const {
	for (int i = 0; i < irreducibleTensors.size() - 1; ++i) {
		if (irreducibleTensors[i].isSameStructure(irreducibleTensors[i + 1])) {
			return true;
		}
	}
	return false;
}

void MathExpressionTerm::getAllAmbiguousZoneLocationsAndSizes(std::vector<std::pair<int, int>>& ambiguousZones) const {
	ambiguousZones.clear();
	int currentZoneLocation = 0;
	int currentZoneSize = 1;
	for (int i = 0; i < irreducibleTensors.size() - 1; ++i) {
		if (irreducibleTensors[i].isSameStructure(irreducibleTensors[i + 1])) {
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

void MathExpressionTerm::getAllPermutationsOfLocationsForAllAmbiguousZones(std::vector<std::vector<std::vector<int>>>& locationPermutationsOfAllZones, std::vector<std::pair<int, int>>& ambiguousZones) const {
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

void MathExpressionTerm::getAllPermutationsOfAllAmbiguousZones(std::vector<MathExpressionTerm>& allIrreducibleTensorTermPermutations) const {
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
		std::vector<IrreducibleTensor> currentArrangementOfIrreducibleTensors(irreducibleTensors);
		//Add the tensors to it from the counter. For each list of permutations:
		for (int j = 0; j < locationPermutationsOfAllZones.size(); ++j) {
			//Replace the Tensors with the permuted version
			std::vector<IrreducibleTensor> permutationToInsert;
			for (int k = 0; k < locationPermutationsOfAllZones[j][0].size(); ++k) {
				int tensorLocation = locationPermutationsOfAllZones[j][locationCounters[j]][k] + ambiguousZones[j].first;
				permutationToInsert.push_back(irreducibleTensors[tensorLocation]);
			}
			//Reflect the current zone in the current Arrangement
			std::copy(permutationToInsert.begin(), permutationToInsert.end(), currentArrangementOfIrreducibleTensors.begin() + ambiguousZones[j].first);
		}
		MathExpressionTerm tempTerm;
		tempTerm.addAllIrreducibleTensors(currentArrangementOfIrreducibleTensors);
		if (leviCevitas.size() == 1) {
			tempTerm.addLeviCivita(leviCevitas[0]);
		}
		tempTerm.setCoefficient(coefficient);
		allIrreducibleTensorTermPermutations.push_back(tempTerm);

		//Increment the first zone's counter
		locationCounters[0]++;
		//Update counter
		for (int j = 0; j < locationCounters.size() - 1; ++j) {
			//If a counter exceeds it's zone's max index, increment the next zone index and reset current to 0.
			if (locationCounters[j] >= locationPermutationsOfAllZones[j].size()) {
				locationCounters[j + 1]++;
				locationCounters[j] = 0;
			}
		}
	}
}

void MathExpressionTerm::groupLevisBySimilarIndices() {
	int leftBound = 0;
	for (int commonIndices = 5; commonIndices >= 0; --commonIndices) {
		for (int i = leftBound; i < leviCevitas.size(); ++i) {
			bool found = false;
			for (int j = i; j < leviCevitas.size(); ++j) {
				if (i != j) {
					if (leviCevitas[i].numberOfSimilarIndices(leviCevitas[j]) == commonIndices) {
						moveLeviAtLocationToLeft(i);
						moveLeviAtLocationToLeft(j);
						leftBound += 2;
						found = true;
						break;
					}
				}
			}
			if (found) {
				break;
			}
		}
		if ((leviCevitas.size() - leftBound) < 3) {
			break;
		}
	}
}

void MathExpressionTerm::moveLeviAtLocationToLeft(int location) {
	LeviCivita temp = leviCevitas[location];
	leviCevitas.erase(leviCevitas.begin() + location);
	leviCevitas.insert(leviCevitas.begin(), temp);
}




void MathExpressionTerm::addAllIrreducibleTensors(std::vector<IrreducibleTensor>& inputIrreducibleTensors) {
	irreducibleTensors = inputIrreducibleTensors;
}


void MathExpressionTerm::addNormalizedTensorFromIrreducibleTensorAtLocation(int irreducibleTensorLocation) {
	NormalizedTensor tempNormalizedTensor(irreducibleTensors[irreducibleTensorLocation].getSymmetryState(), irreducibleTensors[irreducibleTensorLocation].getField());
	tempNormalizedTensor.setIndicesFromIrreducibleTensor(irreducibleTensors[irreducibleTensorLocation]);
	normalizedTensors.push_back(tempNormalizedTensor);
}

void MathExpressionTerm::substituteNormalizedTensorFor1Index0Up1Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 2, 2);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor1Index1Up0Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 2, 2);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}


void MathExpressionTerm::substituteNormalizedTensorFor1Index(int irreducibleTensorLocation) {
	if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 1)) {
		substituteNormalizedTensorFor1Index0Up1Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 1) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 0)) {
		substituteNormalizedTensorFor1Index1Up0Down(irreducibleTensorLocation);
	}
}

void MathExpressionTerm::substituteNormalizedTensorFor2Index0Up0Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 1, 10);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor2Index0Up2Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 1, 2);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor2Index2Up0Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 1, 2);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor2Index1Up1Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 1, 2);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor2Index(int irreducibleTensorLocation) {
	if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 0)) {
		substituteNormalizedTensorFor2Index0Up0Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 2)) {
		substituteNormalizedTensorFor2Index0Up2Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 2) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 0)) {
		substituteNormalizedTensorFor2Index2Up0Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 1) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 1)) {
		substituteNormalizedTensorFor2Index1Up1Down(irreducibleTensorLocation);
	}
}

void MathExpressionTerm::substituteNormalizedTensorFor3Index1Up0Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(4, 3, 3);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor3Index2Up0Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 3, 3);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor3Index2Up1Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(2, 3, 3);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor3Index0Up1Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(4, 3, 3);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor3Index0Up2Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 3, 3);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor3Index1Up2Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(2, 3, 3);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor3Index(int irreducibleTensorLocation) {
	if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 1) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 0)) {
		substituteNormalizedTensorFor3Index1Up0Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 2) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 0)) {
		substituteNormalizedTensorFor3Index2Up0Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 2) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 1)) {
		substituteNormalizedTensorFor3Index2Up1Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 1)) {
		substituteNormalizedTensorFor3Index0Up1Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 2)) {
		substituteNormalizedTensorFor3Index0Up2Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 1) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 2)) {
		substituteNormalizedTensorFor3Index1Up2Down(irreducibleTensorLocation);
	}
}

void MathExpressionTerm::substituteNormalizedTensorFor4Index0Up0Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(4, 3, 15);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor4Index1Up0Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(8, 1, 6);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor4Index0Up1Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(8, 1, 6);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor4Index2Up0Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 1, 2);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor4Index0Up2Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 1, 2);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor4Index1Up1Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 1, 2);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor4Index3Up1Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 3, 6);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor4Index1Up3Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 3, 6);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor4Index2Up2Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(2, 3, 3);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor4Index(int irreducibleTensorLocation) {
	if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 0)) {
		substituteNormalizedTensorFor4Index0Up0Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 1) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 0)) {
		substituteNormalizedTensorFor4Index1Up0Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 1)) {
		substituteNormalizedTensorFor4Index0Up1Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 2) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 0)) {
		substituteNormalizedTensorFor4Index2Up0Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 2)) {
		substituteNormalizedTensorFor4Index0Up2Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 1) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 1)) {
		substituteNormalizedTensorFor4Index1Up1Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 3) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 1)) {
		substituteNormalizedTensorFor4Index3Up1Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 1) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 3)) {
		substituteNormalizedTensorFor4Index1Up3Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 2) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 2)) {
		substituteNormalizedTensorFor4Index2Up2Down(irreducibleTensorLocation);
	}
}

void MathExpressionTerm::substituteNormalizedTensorFor5Index0Up0DownUnbarred(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(2, 15, 15);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor5Index0Up0DownBarred(int irreducibleTensorLocation) {
	NormalizedTensor tempNormalizedTensor(irreducibleTensors[irreducibleTensorLocation].getSymmetryState(), irreducibleTensors[irreducibleTensorLocation].getField());
	tempNormalizedTensor.setIndicesFromIrreducibleTensor(irreducibleTensors[irreducibleTensorLocation]);
	tempNormalizedTensor.setBarred(true);
	normalizedTensors.push_back(tempNormalizedTensor);
	coefficient *= Coefficient(2, 15, 15);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor5Index1Up0Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(4, 5, 10);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor5Index0Up1Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(4, 5, 10);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor5Index0Up2Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 15, 30);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor5Index2Up0Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 15, 30);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor5Index1Up2Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(2, 15, 30);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor5Index2Up1Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(2, 15, 30);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor5Index3Up2Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(2, 15, 15);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor5Index2Up3Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(2, 15, 15);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor5Index(int irreducibleTensorLocation) {
	if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getBarState() == 0)) {
		substituteNormalizedTensorFor5Index0Up0DownUnbarred(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getBarState() == 1)) {
		substituteNormalizedTensorFor5Index0Up0DownBarred(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 1) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 0)) {
		substituteNormalizedTensorFor5Index1Up0Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 1)) {
		substituteNormalizedTensorFor5Index0Up1Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 2)) {
		substituteNormalizedTensorFor5Index0Up2Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 2) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 0)) {
		substituteNormalizedTensorFor5Index2Up0Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 1) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 2)) {
		substituteNormalizedTensorFor5Index1Up2Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 2) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 1)) {
		substituteNormalizedTensorFor5Index2Up1Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 3) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 2)) {
		substituteNormalizedTensorFor5Index3Up2Down(irreducibleTensorLocation);
	}
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 2) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 3)) {
		substituteNormalizedTensorFor5Index2Up3Down(irreducibleTensorLocation);
	}
}

void MathExpressionTerm::substituteNormalizedTensor(int irreducibleTensorLocation) {
	if (irreducibleTensors[irreducibleTensorLocation].getField() == 1) {
		substituteNormalizedTensorFor1Index(irreducibleTensorLocation);
	}
	else if (irreducibleTensors[irreducibleTensorLocation].getField() == 2) {
		substituteNormalizedTensorFor2Index(irreducibleTensorLocation);
	}
	else if (irreducibleTensors[irreducibleTensorLocation].getField() == 3) {
		substituteNormalizedTensorFor3Index(irreducibleTensorLocation);
	}
	else if (irreducibleTensors[irreducibleTensorLocation].getField() == 4) {
		substituteNormalizedTensorFor4Index(irreducibleTensorLocation);
	}
	else if (irreducibleTensors[irreducibleTensorLocation].getField() == 5) {
		substituteNormalizedTensorFor5Index(irreducibleTensorLocation);
	}
}

bool MathExpressionTerm::hasSameDeltas(const MathExpressionTerm& otherMET) const {
	if (deltas.empty() && otherMET.deltas.empty()) {
		return true;
	}
	if (deltas.size() != otherMET.deltas.size()) {
		return false;
	}
	std::vector<Delta> sourceDeltaCopy(deltas);
	std::vector<Delta> otherDeltaCopy(otherMET.deltas);
	std::stable_sort(sourceDeltaCopy.begin(), sourceDeltaCopy.end(), compareDeltasByUpperIndices);
	std::stable_sort(sourceDeltaCopy.begin(), sourceDeltaCopy.end(), compareDeltasByLowerIndices);
	std::stable_sort(otherDeltaCopy.begin(), otherDeltaCopy.end(), compareDeltasByUpperIndices);
	std::stable_sort(otherDeltaCopy.begin(), otherDeltaCopy.end(), compareDeltasByLowerIndices);
	for (int i = 0; i < deltas.size(); ++i) {
		if (!(sourceDeltaCopy[i].isIdentical(otherDeltaCopy[i]))) {
			return false;
		}
	}
	return true;
}

void MathExpressionTerm::renameDeltas(std::vector<std::pair<std::string, std::string>>& renameMap) {
	//for (auto& delta : deltas) {
	//	delta.setUpperIndex(getNewNameFromRenameMap(delta.getUpperIndex(),renameMap));
	//	delta.setLowerIndex(getNewNameFromRenameMap(delta.getLowerIndex(), renameMap));
	//}

	//Trial
	for (auto& delta : deltas) {
		std::string temp(getNewNameFromRenameMap(delta.getUpperIndex(), renameMap));
		if (temp != "") {
			delta.setUpperIndex(temp);
		}
		temp = getNewNameFromRenameMap(delta.getLowerIndex(), renameMap);
		if (temp != "") {
			delta.setLowerIndex(temp);
		}
	}
}

