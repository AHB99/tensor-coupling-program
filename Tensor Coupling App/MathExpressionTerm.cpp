#include "MathExpressionTerm.h"
#include "MathExpression.h"
#include "UtilityFunctions.h"
#include "Spinor.h"
#include "MatterTensor.h"
#include <algorithm>
#include <set>

void MathExpressionTerm::print() const {
	cout << coefficient << "*";
	for (auto beg = deltas.begin(); beg < deltas.end(); ++beg) {
		beg->print();
		cout << "*";
	}
	for (auto beg = leviCivitas.begin(); beg < leviCivitas.end(); ++beg) {
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
	for (auto beg = leviCivitas.begin(); beg < leviCivitas.end(); ++beg) {
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
		leviCivitas.push_back(tempLevi);
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
	leviCivitas.insert(leviCivitas.end(), otherTerm.leviCivitas.begin(), otherTerm.leviCivitas.end());

	//Merge the irreducible tensors
	irreducibleTensors.insert(irreducibleTensors.end(), otherTerm.irreducibleTensors.begin(), otherTerm.irreducibleTensors.end());

	//Phase 2 modification:

	//Merge the normalized tensors
	normalizedTensors.insert(normalizedTensors.end(), otherTerm.normalizedTensors.begin(), otherTerm.normalizedTensors.end());

	//Merge the matter tensors
	matterTensors.insert(matterTensors.end(), otherTerm.matterTensors.begin(), otherTerm.matterTensors.end());
	//Merge the reducible tensors
	reducibleTensors.insert(reducibleTensors.end(), otherTerm.reducibleTensors.begin(), otherTerm.reducibleTensors.end());
	
}

void MathExpressionTerm::sumOverDeltas() {
	bool changeMade = true;
	//Do not attempt if no deltas exist
	while (changeMade&&(!deltas.empty())) {
		//For every delta, check every other delta
		for (int i = 0; i < deltas.size(); ++i) {
			for (int j = 0; j < deltas.size(); ++j) {
				if ((i != j)&&(deltas[i].sumIndicesIfPossible(deltas[j]))) {
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
			if (deltas[i].isCancellationDeltaForIrreducibleTensor(irreducibleTensors[j])) {
				coefficient = 0;
				return true;
			}
		}
	}
	for (int i = 0; i < deltas.size(); ++i) {
		for (int j = 0; j < matterTensors.size(); ++j) {
			if (deltas[i].isCancellationDeltaForMatterTensor(matterTensors[j])) {
				coefficient = 0;
				return true;
			}
		}
	}
	return false;
}

void MathExpressionTerm::renameIrreducibleTensorsByDeltas() {
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
			if ((i != j)&&(deltas[i].isIdentical(deltas[j]))) {
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
	//renameIrreducibleTensorsByDeltas();
	renameAllTensorsAndLevisByDeltas();
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
	leviCivitas.push_back(inpLeviCivita);
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
	for (int i = 0; i < leviCivitas[locationOfFirstLevi].getSize(); ++i) {
		for (int j = 0; j < leviCivitas[locationOfSecondLevi].getSize(); ++j) {
			if (leviCivitas[locationOfFirstLevi].getNameAtLocation(i) == leviCivitas[locationOfSecondLevi].getNameAtLocation(j)) {
				namesInCommon.push_back(leviCivitas[locationOfFirstLevi].getNameAtLocation(i));
				break;
			}
		}
	}
}

void MathExpressionTerm::moveNamesInCommonBetweenLevisToLeft(std::vector<std::string>& namesInCommon, int locationOfFirstLevi, int locationOfSecondLevi) {	
	//Here, "i" also represents the location a particular name must go to in each Levi
	for (int i = 0; i < namesInCommon.size(); ++i) {
		//For first tensor
		int locationOfIndexInLevi = leviCivitas[locationOfFirstLevi].getLocationOfIndex(namesInCommon[i]);
		while (locationOfIndexInLevi != i) {
			leviCivitas[locationOfFirstLevi].swapIndexAtLocationWithOneOnLeft(locationOfIndexInLevi);
			coefficient *= -1;
			--locationOfIndexInLevi;
		}
		//For second tensor
		locationOfIndexInLevi = leviCivitas[locationOfSecondLevi].getLocationOfIndex(namesInCommon[i]);
		while (locationOfIndexInLevi != i) {
			leviCivitas[locationOfSecondLevi].swapIndexAtLocationWithOneOnLeft(locationOfIndexInLevi);
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
	substitutedLeviExpression.setSubstitutionForLeviCivita(termCommonNames.size(), leviCivitas[0], leviCivitas[1], pred2Match, pred1Match, pred0Match);
	return substitutedLeviExpression;
	
}

void MathExpressionTerm::deletePairOfLeviCivitas() {
	leviCivitas.erase(leviCivitas.begin(), leviCivitas.begin()+2);
}

void MathExpressionTerm::reorderIndicesOfIrreducibleTensorsAndLeviCivitas() {
	for (auto& tensor : irreducibleTensors) {
		//Reorder upper indices
		bool modified = true;
		while (modified) {
			modified = false;
			//Check till second last index
			for (int i = 0; i < tensor.getNumberOfUpperIndices() - 1; ++i) {
				if ((tensor.getUpperIndexAt(i)>tensor.getUpperIndexAt(i + 1))) {
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
				if ((tensor.getLowerIndexAt(i)>tensor.getLowerIndexAt(i + 1))) {
					tensor.swapLowerIndexAtLocationWithOneOnRight(i);
					if (!tensor.getSymmetryState()) {
						coefficient *= -1;
					}
					modified = true;
				}

			}
		}
	}
	for (auto& levi : leviCivitas) {
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
	if (leviCivitas.size() == 1) {
		if (leviCivitas[0].doesIndexExist(index)) {
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
	if (leviCivitas.size() == 1) {
		for (int i = 0; i < leviCivitas[0].getSize(); ++i) {
			std::string currentName = leviCivitas[0].getNameAtLocation(i);
			if ((result.empty()) || (std::find(result.begin(), result.end(), currentName) == result.end())) {
				result.push_back(currentName);
			}
		}
	}
	return result;
}

void MathExpressionTerm::performRenamesOnIrreducibleTensors(const std::vector<std::pair<std::string, std::string>>& renameMap) {
	for (auto& tensor : irreducibleTensors) {
		//Change each upper index
		for (int i = 0; i < tensor.getNumberOfUpperIndices(); ++i) {
			tensor.changeUpperIndexAtLocationTo(i,getNewNameFromRenameMap(tensor.getUpperIndexAt(i), renameMap));
		}
		//Change each lower index
		for (int i = 0; i < tensor.getNumberOfLowerIndices(); ++i) {
			tensor.changeLowerIndexAtLocationTo(i, getNewNameFromRenameMap(tensor.getLowerIndexAt(i), renameMap));
		}
	}
}

void MathExpressionTerm::performRenamesOnLeviCivita(const std::vector<std::pair<std::string, std::string>>& renameMap) {
	for (int i = 0; i < leviCivitas[0].getSize(); ++i) {
		leviCivitas[0].changeIndexAtLocationTo(i, getNewNameFromRenameMap(leviCivitas[0].getNameAtLocation(i), renameMap));
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
	for (auto& possiblySymmetricTensor : irreducibleTensors) {
		//For every symmetric Tensor
		if (possiblySymmetricTensor.getSymmetryState()) {
			//Check every matter Tensor
			for (auto& matterTensor : matterTensors) {
				if (possiblySymmetricTensor.hasAtleast2MatchingIndices(matterTensor)) {
					coefficient = 0;
					return false;
				}
			}
		}
	}

	return true;
}

bool MathExpressionTerm::isAmbiguous() const {
	for (int i = 0; i < irreducibleTensors.size()-1; ++i) {
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
		if (irreducibleTensors[i].isSameStructure(irreducibleTensors[i+1])) {
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
		if (leviCivitas.size() == 1) {
			tempTerm.addLeviCivita(leviCivitas[0]);
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
		for (int i = leftBound; i < leviCivitas.size(); ++i) {
			bool found = false;
			for (int j = i; j < leviCivitas.size(); ++j) {
				if (i != j) {
					if (leviCivitas[i].numberOfSimilarIndices(leviCivitas[j]) == commonIndices) {
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
		if ((leviCivitas.size() - leftBound) < 3) {
			break;
		}
	}
}

void MathExpressionTerm::moveLeviAtLocationToLeft(int location) {
	LeviCivita temp = leviCivitas[location];
	leviCivitas.erase(leviCivitas.begin() + location);
	leviCivitas.insert(leviCivitas.begin(), temp);
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
	coefficient *= Coefficient(1, 1, 2);
	irreducibleTensors.erase(irreducibleTensors.begin() + irreducibleTensorLocation);
}

void MathExpressionTerm::substituteNormalizedTensorFor1Index1Up0Down(int irreducibleTensorLocation) {
	addNormalizedTensorFromIrreducibleTensorAtLocation(irreducibleTensorLocation);
	coefficient *= Coefficient(1, 1, 2);
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
	coefficient *= Coefficient(1, 3, 6);
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

//TEST: New function
void MathExpressionTerm::substituteNormalizedTensorFor5Index0Up3Down(int irreducibleTensorLocation) {
	NormalizedTensor tempNormalizedTensor(irreducibleTensors[irreducibleTensorLocation].getSymmetryState(), irreducibleTensors[irreducibleTensorLocation].getField());
	normalizedTensors.push_back(tempNormalizedTensor);

	std::vector<std::string> newNames{};
	newNames.push_back(getNextNameGivenTerm(*this));
	normalizedTensors[0].addUpperIndex(newNames[0]);
	newNames.push_back(getNextNameGivenTerm(*this));
	normalizedTensors[0].addUpperIndex(newNames[1]);

	LeviCivita tempLevi(false);
	tempLevi.addIndex(irreducibleTensors[irreducibleTensorLocation].getLowerIndexAt(0));
	tempLevi.addIndex(irreducibleTensors[irreducibleTensorLocation].getLowerIndexAt(1));
	tempLevi.addIndex(irreducibleTensors[irreducibleTensorLocation].getLowerIndexAt(2));
	tempLevi.addIndex(newNames[0]);
	tempLevi.addIndex(newNames[1]);

	
	leviCivitas.push_back(tempLevi);
	coefficient *= Coefficient(1, 15, 30);
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
	//newly added
	else if ((irreducibleTensors[irreducibleTensorLocation].getNumberOfUpperIndices() == 0) && (irreducibleTensors[irreducibleTensorLocation].getNumberOfLowerIndices() == 3)) {
		substituteNormalizedTensorFor5Index0Up3Down(irreducibleTensorLocation);
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

//*********************Phase 2******************************
void MathExpressionTerm::printPhase2() const {
	print();
	if (!fab.getIsNull()) {
		fab.print();
	}
	if (hasChargeConjugate) {
		if (spinors.size() != 2) {
			cout << "<0|";
		}
		else {
			spinors[0].print();
		}
		leftBbtChain.print();
		if (hasChargeConjugate) {
			cout << " B ";
		}
		gammaTensor.print();
		rightBbtChain.print();
		if (spinors.size() != 2) {
			cout << "|0>";
		}
		else {
			spinors[1].print();
		}
	}
	//If no charge conjugate, ignore bbt section completely, because charge conjugate has evaluated it.
	else {
		//Also, if no conjugate, add 'i' from B = -i * PI
		cout << "*i";
	}
	for (auto& matterTensor : matterTensors) {
		cout << "*";
		matterTensor.print();
	}
	for (int i = 0; i < reducibleTensors.size(); ++i) {
		cout << "*";
		reducibleTensors[i].printTensor();
	}
	cout << endl;

}

void MathExpressionTerm::printLatexPhase2() const {
	coefficient.printLatex();
	if (!fab.getIsNull()) {
		fab.printLatex();
	}
	if (hasChargeConjugate) {
		if (spinors.size() != 2) {
			cout << "<0|";
		}
		else {
			spinors[0].printLatex();
		}
		leftBbtChain.printLatex();
		if (hasChargeConjugate) {
			cout << "B";
		}
		gammaTensor.printLatex();
		rightBbtChain.printLatex();
		if (spinors.size() != 2) {
			cout << "|0>";
		}
		else {
			spinors[1].printLatex();
		}
	}
	//If no charge conjugate, ignore bbt section completely, because charge conjugate has evaluated it.
	else {
		//Also, if no conjugate, add 'i' from B = -i * PI
		cout << "i";
	}
	for (auto& delta : deltas) {
		delta.printLatex();
	}
	for (auto& levi : leviCivitas) {
		levi.printLatex();
	}
	for (auto& irreducibleTensor : irreducibleTensors) {
		irreducibleTensor.printLatex();
	}
	for (auto& normalizedTensor : normalizedTensors) {
		normalizedTensor.printLatex();
	}
	for (auto& matterTensor : matterTensors) {
		matterTensor.printLatex();
	}
	for (auto& reducibleTensor : reducibleTensors) {
		reducibleTensor.printLatex();
	}
	cout << endl;
}

void MathExpressionTerm::inputByUserPhase2() {
	inputByUser();
	fab.inputByUser();

	int tempNum;
	cout << "Has Spinors? (0 = No, 1 = Yes): ";
	cin >> tempNum;
	if (tempNum == 1) {
		cout << "Left Spinor: \n";
		Spinor tempSpinor;
		tempSpinor.setIsLeft(true);
		tempSpinor.inputByUser();
		spinors.push_back(tempSpinor);

		cout << "Right Spinor: \n";
		Spinor tempSpinor2;
		tempSpinor2.setIsLeft(false);
		tempSpinor2.inputByUser();
		spinors.push_back(tempSpinor2);
	}

	gammaTensor.inputByUser();

	cout << "Left Bbt Chain" << endl;
	leftBbtChain.inputByUser();
	cout << "Right Bbt Chain" << endl;
	rightBbtChain.inputByUser();

	cout << "Number of Matter Tensors: ";
	cin >> tempNum;
	for (int i = 0; i < tempNum; ++i) {
		MatterTensor tempMat;
		tempMat.inputByUser();
		matterTensors.push_back(tempMat);
	}

	cout << "Has charge conjugate? (0 = No, 1 = Yes): ";
	cin >> hasChargeConjugate;

	cout << "Enter number of Reducible tensors: ";
	cin >> tempNum;
	for (int i = 0; i < tempNum; ++i) {
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
		reducibleTensors.push_back(tempTensor);
	}

}

std::vector<std::string> MathExpressionTerm::getInitialIndicesFromGamma() const {
	return gammaTensor.getIndices();
}

void MathExpressionTerm::fillBbtIndicesFromGamma() {
	std::vector<std::string> allIndexNames = getInitialIndicesFromGamma();
	for (auto& index : allIndexNames) {
		rightBbtChain.addBbt(index, false);
	}
}



void MathExpressionTerm::fillBbtDaggersAndReducibleTensorBarsBasedOnBinaryMarker(std::string indexName, int binaryMarker) {
	for (auto& tensor : reducibleTensors) {
		//For every index in every tensor
		for (int i = 0; i < tensor.getNumberOfIndices(); ++i) {
			//If we find a match with the target name
			if (tensor.getIndexNameAt(i) == indexName) {
				if (binaryMarker == 0) {
					tensor.setBarStateAt(0, i);
				}
				else if (binaryMarker == 1) {
					tensor.setBarStateAt(1, i);
				}
			}
		}
	}
	for (int i = 0; i < rightBbtChain.getSize(); ++i) {
		//If we find a match with the target name
		if (rightBbtChain.getBbtNameAt(i) == indexName) {
			if (binaryMarker == 0) {
				rightBbtChain.setBbtDaggerStateAt(1, i);
			}
			else if (binaryMarker == 1) {
				rightBbtChain.setBbtDaggerStateAt(0, i);
			}
		}
	}
	
}

void MathExpressionTerm::removeRightBbtChainBbtAtLocation(int location) {
	rightBbtChain.removeBbtAtLocation(location);
}


MathExpression MathExpressionTerm::shiftRightBbtChainBtOperatorsToLeft() {
	MathExpression resultExpression;
	bool modified = true;
	while (modified) {
		modified = false;
		//Check till second last index
		for (int i = 0; i < rightBbtChain.getSize() - 1; ++i) {
			//If current is b and next is bt
			if ((!rightBbtChain.isDaggerAt(i) && rightBbtChain.isDaggerAt(i + 1))) {
				rightBbtChain.swapBbtAtLocationWithOneOnRight(i);

				//TODO: Generate term, BEFORE COEFFICIENT IS CHANGED
				MathExpressionTerm generatedTerm(*this);
				Delta generatedDelta(rightBbtChain.getBbtNameAt(i), rightBbtChain.getBbtNameAt(i + 1));
				generatedTerm.addDelta(generatedDelta);
				//Because of vector shifting, retain location
				generatedTerm.removeRightBbtChainBbtAtLocation(i);
				generatedTerm.removeRightBbtChainBbtAtLocation(i);

				resultExpression.addTerm(generatedTerm);

				coefficient *= -1;

				modified = true;
			}

		}
	}
	return resultExpression;
}

void MathExpressionTerm::simplifyTermWithReducibleTensorsByDeltas() {
	sumOverDeltas();
	renameReducibleTensorsByDeltasIfPossible();
	solveMatchingDeltas();
}

void MathExpressionTerm::renameReducibleTensorsByDeltasIfPossible() {
	for (int i = 0; i < deltas.size(); ++i) {
		for (int j = 0; j < reducibleTensors.size(); ++j) {
			//If a rename occured
			if (deltas[i].replaceIndexIfPossible(reducibleTensors[j])) {
				//Delete that delta
				deltas.erase(deltas.begin() + i);
				//Decrement 'i' so it stays in the same position for next unchecked delta
				--i;
				break;
			}
		}
	}
}


TensorTerm MathExpressionTerm::convertToTensorTermEquivalent() const {
	TensorTerm resultReducibleTensorTerm;

	//First dump all existing reducible tensors
	for (auto& existingReducibleTensor : reducibleTensors) {
		resultReducibleTensorTerm.addTensor(existingReducibleTensor);
	}

	//Convert the right bbtChain to a Reducible Tensor
	resultReducibleTensorTerm.addTensor(rightBbtChain.convertToReducibleTensorEquivalent());
	resultReducibleTensorTerm.setCoefficient(coefficient);
	return resultReducibleTensorTerm;
}

MathExpression MathExpressionTerm::performPsiSubstitution(std::string& latestNameMain) {
	MathExpression finalSubstituted;
	MathExpression leftSubstituted;

	if (spinors[0].getIsStarred()) {
		leftSubstituted = substitutePsiForLeftStarred(latestNameMain);
	}
	for (int i = 0; i < leftSubstituted.getSize(); ++i) {
		if (!spinors[1].getIsStarred()) {
			std::string latestName(latestNameMain);
			MathExpression rightSubstituted;
			rightSubstituted = leftSubstituted.getTermAtLocation(i).substitutePsiForRightUnstarred(latestName);
			finalSubstituted += rightSubstituted;
		}
	}
	return finalSubstituted;
}

MathExpression MathExpressionTerm::substitutePsiForLeftStarred(std::string& latestName) {
	MathExpression resultExpression;
	MathExpressionTerm originalTerm(*this);
	std::vector<MathExpressionTerm> tempMathExpressionTerms(3, originalTerm);
	std::vector<MatterTensor> tempMatterTensors(3);

	//Create 5 new names to use
	std::vector<std::string> newNames;
	for (int i = 0; i < 5; ++i) {
		latestName = getNextNameGivenNamePhase2(latestName);
		newNames.push_back(latestName);
	}
	
	//Term 1
	tempMatterTensors[0].addLowerIndex(spinors[0].getIndex());
	tempMatterTensors[0].setIsTranspose(true);
	tempMathExpressionTerms[0].addMatterTensor(tempMatterTensors[0]);

	//Term 2
	tempMatterTensors[1].addLowerIndex(spinors[0].getIndex());
	tempMatterTensors[1].addUpperIndex(newNames[0]);
	tempMatterTensors[1].addUpperIndex(newNames[1]);
	tempMatterTensors[1].setIsTranspose(true);
	tempMathExpressionTerms[1].addMatterTensor(tempMatterTensors[1]);
	tempMathExpressionTerms[1].multiplyWithCoefficient(Coefficient(1, 2));
	tempMathExpressionTerms[1].addBbtToLeftChain(newNames[1], false);
	tempMathExpressionTerms[1].addBbtToLeftChain(newNames[0], false);

	//Term 3
	tempMatterTensors[2].addLowerIndex(spinors[0].getIndex());
	tempMatterTensors[2].addLowerIndex(newNames[0]);
	tempMatterTensors[2].setIsTranspose(true);
	tempMathExpressionTerms[2].addMatterTensor(tempMatterTensors[2]);
	LeviCivita tempLevi;
	for (auto& name : newNames) {
		tempLevi.addIndex(name);
	}
	tempMathExpressionTerms[2].addLeviCivita(tempLevi);
	tempMathExpressionTerms[2].multiplyWithCoefficient(Coefficient(1, 24));
	for (int i = 4; i >= 1; --i) {
		tempMathExpressionTerms[2].addBbtToLeftChain(newNames[i], false);
	}
	for (auto& term : tempMathExpressionTerms) {
		term.removeLeftSpinor();
		resultExpression.addTerm(term);
	}

	return resultExpression;
}

MathExpression MathExpressionTerm::substitutePsiForRightUnstarred(std::string& latestName) {
	MathExpression resultExpression;
	MathExpressionTerm originalTerm(*this);
	std::vector<MathExpressionTerm> tempMathExpressionTerms(3, originalTerm);
	std::vector<MatterTensor> tempMatterTensors(3);

	//Create 5 new names to use
	std::vector<std::string> newNames;
	for (int i = 0; i < 5; ++i) {
		latestName = getNextNameGivenNamePhase2(latestName);
		newNames.push_back(latestName);
	}

	//Term 1
	tempMatterTensors[0].addLowerIndex(spinors[0].getIndex());
	tempMatterTensors[0].setIsTranspose(false);
	tempMathExpressionTerms[0].addMatterTensor(tempMatterTensors[0]);

	//Term 2
	tempMatterTensors[1].addLowerIndex(spinors[0].getIndex());
	tempMatterTensors[1].addUpperIndex(newNames[0]);
	tempMatterTensors[1].addUpperIndex(newNames[1]);
	tempMatterTensors[1].setIsTranspose(false);
	tempMathExpressionTerms[1].addMatterTensor(tempMatterTensors[1]);
	tempMathExpressionTerms[1].multiplyWithCoefficient(Coefficient(1, 2));
	tempMathExpressionTerms[1].addBbtToRightChain(newNames[0], true);
	tempMathExpressionTerms[1].addBbtToRightChain(newNames[1], true);

	//Term 3
	tempMatterTensors[2].addLowerIndex(spinors[0].getIndex());
	tempMatterTensors[2].addLowerIndex(newNames[0]);
	tempMatterTensors[2].setIsTranspose(false);
	tempMathExpressionTerms[2].addMatterTensor(tempMatterTensors[2]);
	LeviCivita tempLevi;
	for (auto& name : newNames) {
		tempLevi.addIndex(name);
	}
	tempMathExpressionTerms[2].addLeviCivita(tempLevi);
	tempMathExpressionTerms[2].multiplyWithCoefficient(Coefficient(1, 24));
	for (int i = 1; i <= 4; ++i) {
		tempMathExpressionTerms[2].addBbtToRightChain(newNames[i], true);
	}
	for (auto& term : tempMathExpressionTerms) {
		term.removeRightSpinor();
		resultExpression.addTerm(term);
	}

	return resultExpression;

}

void MathExpressionTerm::removeLeftSpinor() {
	spinors.erase(std::remove_if(spinors.begin(), spinors.end(), [](const Spinor& s) -> bool {return s.getIsLeft(); }), spinors.end());
}
void MathExpressionTerm::removeRightSpinor() {
	spinors.erase(std::remove_if(spinors.begin(), spinors.end(), [](const Spinor& s) -> bool {return !s.getIsLeft(); }), spinors.end());
}

bool MathExpressionTerm::willBbtFormUsefulInteraction() {
	BbtChainCounter bbtChainCounter = rightBbtChain.getBbtChainCounter();

	//More bs than bts, will result in 0 term.
	if (bbtChainCounter.bCount > bbtChainCounter.btAfterBCount) {
		return false;
	}

	int btBeforeBCount = bbtChainCounter.btTotalCount - bbtChainCounter.btAfterBCount;
	int rightmostBtsAfterCancellations = bbtChainCounter.btAfterBCount - bbtChainCounter.bCount;
	int totalOperators = btBeforeBCount + rightmostBtsAfterCancellations + leftBbtChain.getSize();
	if (totalOperators == 5) {
		return true;
	}
	else {
		return false;
	}

}

std::vector<std::string> MathExpressionTerm::getAllUniqueNamesOfTerm() const {
	std::set<std::string> nameSet;
	for (auto& levi : leviCivitas) {
		std::vector<std::string> leviIndices{ levi.getIndices() };
		nameSet.insert(leviIndices.begin(), leviIndices.end());
	}
	for (auto& irreducibleTensor : irreducibleTensors) {
		std::vector<std::string> irreducibleTensorIndices{ irreducibleTensor.getIndices() };
		nameSet.insert(irreducibleTensorIndices.begin(), irreducibleTensorIndices.end());
	}
	for (auto& normalizedTensor : normalizedTensors) {
		std::vector<std::string> normalizedTensorIndices{ normalizedTensor.getIndices() };
		nameSet.insert(normalizedTensorIndices.begin(), normalizedTensorIndices.end());
	}

	std::vector<std::string> fabIndices{ fab.getIndices() };
	nameSet.insert(fabIndices.begin(), fabIndices.end());

	std::vector<std::string> leftBbtIndices{ leftBbtChain.getIndices() };
	nameSet.insert(leftBbtIndices.begin(), leftBbtIndices.end());

	std::vector<std::string> rightBbtIndices{ rightBbtChain.getIndices() };
	nameSet.insert(rightBbtIndices.begin(), rightBbtIndices.end());

	for (auto& matterTensor : matterTensors) {
		std::vector<std::string> matterTensorIndices{ matterTensor.getIndices() };
		nameSet.insert(matterTensorIndices.begin(), matterTensorIndices.end());
	}

	for (auto& reducibleTensor : reducibleTensors) {
		std::vector<std::string> reducibleTensorIndices{ reducibleTensor.getIndices() };
		nameSet.insert(reducibleTensorIndices.begin(), reducibleTensorIndices.end());
	}

	return std::vector<std::string>(nameSet.begin(), nameSet.end());

}

void MathExpressionTerm::reduceRightBbtChainByEvaluation() {
	BbtChainCounter bbtChainCounter = rightBbtChain.getBbtChainCounter();
	//If no b's, then no further work needed.
	if (bbtChainCounter.bCount == 0) return;

	int locationOfFirstBOperator = bbtChainCounter.btTotalCount - bbtChainCounter.btAfterBCount;
	//Assuming #bs < #bts, then we symmetrically jump to the last bbt that will be coupled
	int locationOfLastCoupledBbtOperator = locationOfFirstBOperator + (bbtChainCounter.bCount * 2) - 1;

	generateDeltasByRightBbtChainEvaluation(locationOfFirstBOperator, locationOfLastCoupledBbtOperator);

	deleteBbtsByRightBbtChainEvaluation(locationOfFirstBOperator, locationOfLastCoupledBbtOperator);

	Coefficient btCoefficient( combination(bbtChainCounter.btAfterBCount,bbtChainCounter.btAfterBCount - bbtChainCounter.bCount) );
	Coefficient deltaCoefficient(factorial(bbtChainCounter.bCount));

	multiplyWithCoefficient(btCoefficient);
	multiplyWithCoefficient(deltaCoefficient);


}


void MathExpressionTerm::generateDeltasByRightBbtChainEvaluation(int locationOfFirstBOperator, int locationOfLastCoupledBbtOperator) {
	int i = locationOfFirstBOperator, j = locationOfLastCoupledBbtOperator;
	while (i < j) {
		Delta tempDelta(rightBbtChain.getBbtNameAt(i), rightBbtChain.getBbtNameAt(j));
		deltas.push_back(tempDelta);
		++i;
		--j;
	}	
}

void MathExpressionTerm::deleteBbtsByRightBbtChainEvaluation(int locationOfFirstBOperator, int locationOfLastCoupledBbtOperator) {
	rightBbtChain.deleteBbtByRange(locationOfFirstBOperator, locationOfLastCoupledBbtOperator+1);
}

void MathExpressionTerm::evaluateChargeConjugate() {
	LeviCivita leviToAdd(false);

	//Adding leftbbts then rightbbts, both in reverse.
	for (int i = leftBbtChain.getSize() - 1; i >= 0; --i) {
		leviToAdd.addIndex(leftBbtChain.getBbtNameAt(i));
	}
	for (int i = rightBbtChain.getSize() - 1; i >= 0; --i) {
		leviToAdd.addIndex(rightBbtChain.getBbtNameAt(i));
	}

	//If odd number of b, negative
	if (leftBbtChain.getSize() % 2 != 0) { multiplyWithCoefficient(Coefficient(-1)); }

	addLeviCivita(leviToAdd);

	//Now entire bbt section is considered invalid
	hasChargeConjugate = false;
	rightBbtChain.clear();
	leftBbtChain.clear();
	//Upon removal, add -i
	multiplyWithCoefficient(Coefficient(-1));
}

void MathExpressionTerm::renameAllTensorsAndLevisByDeltas() {
	renameIrreducibleTensorsByDeltas();
	for (int i = 0; i < deltas.size(); ++i) {
		for (int j = 0; j < matterTensors.size(); ++j) {
			//If a rename occured
			if (deltas[i].replaceIndexIfPossible(matterTensors[j])) {
				//Delete that delta
				deltas.erase(deltas.begin() + i);
				//Decrement 'i' so it stays in the same position for next unchecked delta
				--i;
				break;
			}
		}
	}
	for (int i = 0; i < deltas.size(); ++i) {
		for (int j = 0; j < leviCivitas.size(); ++j) {
			//If a rename occured
			if (deltas[i].replaceIndexIfPossible(leviCivitas[j])) {
				//Delete that delta
				deltas.erase(deltas.begin() + i);
				//Decrement 'i' so it stays in the same position for next unchecked delta
				--i;
				break;
			}
		}
	}
}


void MathExpressionTerm::sortIrreducibleAndMatterTensors() {
	sortIrreducibleTensors();
	std::stable_sort(matterTensors.begin(), matterTensors.end(), [](const MatterTensor& lhs, const MatterTensor& rhs) -> bool {return lhs.getNumberOfUpperIndices() < rhs.getNumberOfUpperIndices(); });
	std::stable_sort(matterTensors.begin(), matterTensors.end(), [](const MatterTensor& lhs, const MatterTensor& rhs) -> bool {return lhs.getNumberOfLowerIndices() < rhs.getNumberOfLowerIndices(); });
}

void MathExpressionTerm::reorderIndicesOfAllTensors() {
	int antisymmetricCoefficient = 1;
	reorderIndicesOfIrreducibleTensorsAndLeviCivitas();
	for (auto& matterTensor : matterTensors) {
		antisymmetricCoefficient *= matterTensor.reorderIndices();
	}
	if (!fab.getIsNull()) {
		antisymmetricCoefficient *= fab.reorderIndices();
	}
	multiplyWithCoefficient(Coefficient(antisymmetricCoefficient));
}


LocationOfIndexPojo MathExpressionTerm::getLocationOfFirstOccurenceOfIndex(const std::string& index) const {
	if (leviCivitas.size() == 1) {
		if (leviCivitas[0].doesIndexExist(index)) {
			return LocationOfIndexPojo(TensorType::LEVI,0, false);
		}
	}
	if (!fab.getIsNull()) {
		if (fab.doesIndexExist(index)) {
			return LocationOfIndexPojo(TensorType::FAB, 0, false);
		}
	}
	for (int i = 0; i < irreducibleTensors.size(); ++i) {
		if (irreducibleTensors[i].doesIndexExistInUpperZone(index)) {
			return LocationOfIndexPojo(TensorType::IRREDUCIBLE_TENSOR, i, true);
		}
		else if (irreducibleTensors[i].doesIndexExistInLowerZone(index)){
			return LocationOfIndexPojo(TensorType::IRREDUCIBLE_TENSOR, i, false);
		}
	}
	for (int i = 0; i < matterTensors.size(); ++i) {
		if (matterTensors[i].doesIndexExistInUpperZone(index)) {
			return LocationOfIndexPojo(TensorType::MATTER_TENSOR, i, true);
		}
		else if (matterTensors[i].doesIndexExistInLowerZone(index)) {
			return LocationOfIndexPojo(TensorType::MATTER_TENSOR, i, false);
		}
	}

	return LocationOfIndexPojo(TensorType::IRREDUCIBLE_TENSOR, -1, false);
}

void MathExpressionTerm::performRenamesOnMatterTensors(const std::vector<std::pair<std::string, std::string>>& renameMap) {
	for (auto& tensor : matterTensors) {
		//Change each upper index
		for (int i = 0; i < tensor.getNumberOfUpperIndices(); ++i) {
			tensor.changeUpperIndexAtLoc(i, getNewNameFromRenameMap(tensor.getUpperIndexAt(i), renameMap));
		}
		//Change each lower index
		for (int i = 0; i < tensor.getNumberOfLowerIndices(); ++i) {
			tensor.changeLowerIndexAtLoc(i, getNewNameFromRenameMap(tensor.getLowerIndexAt(i), renameMap));
		}
	}
}

void MathExpressionTerm::performRenamesOnFab(const std::vector<std::pair<std::string, std::string>>& renameMap) {
	for (int i = 0; i < fab.getNumberOfIndices(); ++i) {
		fab.changeIndexAt(i, getNewNameFromRenameMap(fab.getIndexAt(i), renameMap));
	}
}

void MathExpressionTerm::performRenamesPhase2(const std::vector<std::pair<std::string, std::string>>& renameMap) {
	performRenamesOnIrreducibleTensors(renameMap);
	if (getNumberOfLeviCivitas() == 1) {
		performRenamesOnLeviCivita(renameMap);
	}
	performRenamesOnMatterTensors(renameMap);
	performRenamesOnFab(renameMap);
}

void MathExpressionTerm::chargeFab() {
	multiplyWithCoefficient(2);
	if (fab.getField() == 1 || fab.getField() == 5) {
		fab.setCharge(FabCharge::POSITIVE);
	}
	else if (fab.getField() == 3) {
		fab.setCharge(FabCharge::NEGATIVE);
	}

}

void MathExpressionTerm::mergeFabs(const MathExpressionTerm& rhs) {
	//If not mismatched, fail
	if (fab.getIndexAt(0) == rhs.fab.getIndexAt(0)) return;
	//If not equal coefficients, fail
	if (coefficient.getAbsValue() != rhs.coefficient.getAbsValue()) return;
	//If signs incompatible, fail
	if (fab.getField() == 1 || fab.getField() == 5) {
		if ((coefficient * rhs.coefficient) < 0) return;
	}
	else if (fab.getField() == 3) {
		if ((coefficient * rhs.coefficient) > 0) return;
	}

	//If all tests pass, charge fab
	chargeFab();

}
