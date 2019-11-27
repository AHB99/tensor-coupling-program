#include "UtilityFunctions.h"
#include "IrreducibleTensor.h"


//Non-member function definitions

bool checkZoneRenameIssue(TensorTerm& sourceTerm, TensorTerm& attemptTerm, std::pair<std::string, std::string> rename) {
	for (int i = 0; i < attemptTerm.getSize(); ++i) {
		//if the old name exists in the attempt term but the new name does not exist in the source term.
		if ((attemptTerm.getTensor(i).doesNameExistInBarredZone(rename.first)) && (!(sourceTerm.getTensor(i).doesNameExistInBarredZone(rename.second)))) {
			return false;
		}
		if ((attemptTerm.getTensor(i).doesNameExistInUnbarredZone(rename.first)) && (!(sourceTerm.getTensor(i).doesNameExistInUnbarredZone(rename.second)))) {
			return false;
		}
	}
	return true;
}

bool performRenameIfValid(TensorTerm& sourceTerm, TensorTerm& attemptTerm, TensorTerm& renamedTerm) {
	////Aug2nd debug
	//Notice, initial mapping should be attemptTerms names. Previous method worked by coincidence.
	//std::vector<std::string> allSourceNames = sourceTerm.getListOfNames();
	std::vector<std::string> allSourceNames = attemptTerm.getListOfNames();
	////end Aug2nd debug
	renamedTerm = attemptTerm;
	std::vector<std::pair<std::string, std::string>> renameMap;
	//Fill map with indices and empty assignments
	for (auto& sourceName : allSourceNames) {
		renameMap.push_back(std::make_pair(sourceName, ""));
	}
	//For each index
	for (int i = 0; i < renameMap.size(); ++i) {
		int tensorLocation = renamedTerm.findTensorLocationOfFirstOccurenceOfIndex(renameMap[i].first);
		int barstateOfIndex = renamedTerm.getTensor(tensorLocation).getBarStateFromName(renameMap[i].first);

		int barredZoneBorder = sourceTerm.getTensor(tensorLocation).getBarredZoneLocation();
		int unBarredZoneBorder = sourceTerm.getTensor(tensorLocation).getUnbarredZoneLocation();

		if (barstateOfIndex == 1) {
			//For each index in the corresponding Source Term Zone
			for (int j = 0; j <= barredZoneBorder; ++j) {
				std::string sourceTermNewName = sourceTerm.getTensor(tensorLocation).getIndexNameAt(j);
				std::pair<std::string, std::string> candidateRename(renameMap[i].first, sourceTermNewName);
				//If it's an available assignment and the rename will not cause any issues, then assign it
				bool b1 = isNameAlreadyAssigned(renameMap, sourceTermNewName);
				bool b2 = checkZoneRenameIssue(sourceTerm, attemptTerm, candidateRename);
				if ((!b1) && (b2)) {
					renameMap[i] = candidateRename;
					break;
				}
				else {
					if (j == barredZoneBorder) {
						//No possible renames left
						return false;
					}
				}
			}
		}
		else if (barstateOfIndex == 0) {
			//For each index in the corresponding Source Term Zone
			for (int j = unBarredZoneBorder; j < sourceTerm.getTensor(tensorLocation).getNumberOfIndices(); ++j) {
				std::string sourceTermNewName = sourceTerm.getTensor(tensorLocation).getIndexNameAt(j);
				std::pair<std::string, std::string> candidateRename(renameMap[i].first, sourceTermNewName);
				//If it's an available assignment and the rename will not cause any issues, then assign it
				bool b1 = isNameAlreadyAssigned(renameMap, sourceTermNewName);
				bool b2 = checkZoneRenameIssue(sourceTerm, attemptTerm, candidateRename);
				if ((!b1) && (b2)) {
					renameMap[i] = candidateRename;
					break;
				}
				else {
					if (j == sourceTerm.getTensor(tensorLocation).getNumberOfIndices() - 1) {
						//No possible renames left
						return false;
					}
				}
			}
		}
	}
	//Perform all renames
	//For every tensor
	for (int i = 0; i < renamedTerm.getSize(); ++i) {
		//For every index
		for (int j = 0; j < renamedTerm.getTensor(i).getNumberOfIndices(); ++j) {
			std::string oldName = renamedTerm.getTensor(i).getIndexNameAt(j);
			std::string newName;
			//Find the new name from rename map
			for (int k = 0; k < renameMap.size(); ++k) {
				if (renameMap[k].first == oldName) {
					newName = renameMap[k].second;
					break;
				}
			}
			renamedTerm.changeIndexNameTo(i, j, newName);
		}
	}


	return true;
}

bool isNameAlreadyAssigned(std::vector<std::pair<std::string, std::string>>& renameMap, std::string newName) {
	for (auto& rename : renameMap) {
		if (rename.second == newName) {
			return true;
		}
	}
	return false;
}

bool compareTensorsByNumberOfBarredIndices(const Tensor& lhs, const Tensor& rhs) {
	return (lhs.getNumberOfBarredIndices() < rhs.getNumberOfBarredIndices());
}

bool compareTensorsByNumberOfIndices(const Tensor& lhs, const Tensor& rhs) {
	return (lhs.getNumberOfIndices() < rhs.getNumberOfIndices());
}


bool compareTensorsByLabel(const Tensor& lhs, const Tensor& rhs) {
	return (lhs.getLabel() < rhs.getLabel());
}

std::vector<int> findAllPermutationsOfLocationsGivenSize(int size) {
	std::vector<int> originalArrangement, resultVector;
	for (int i = 0; i < size; ++i) {
		originalArrangement.push_back(i);
	}

	for (int i = 0; i < factorial(size); ++i) {
		std::next_permutation(originalArrangement.begin(), originalArrangement.end());
		resultVector.insert(resultVector.end(), originalArrangement.begin(), originalArrangement.end());
	}
	return resultVector;
}

int factorial(int n) {
	int result = 1;
	if ((n == 0) || (n == 1)) {
		return result;
	}
	else {
		for (int i = n; i > 1; --i) {
			result *= i;
		}
	}
	return result;
}

bool performRenameIfValidIncludingPermutations(TensorTerm& sourceTerm, TensorTerm& attemptTerm, TensorTerm& renamedTerm) {
	//If sourceTerm (and hence also attemptTerm, since structure is same) is ambiguous
	if (sourceTerm.isAmbiguous()) {
		//Generate all possible arrangments for the attemptTerm, and try to match them with the sourceTerm
		std::vector<TensorTerm> allPossiblePermutationsOfAttemptTerm;
		attemptTerm.getAllPermutationsOfAllAmbiguousZones(allPossiblePermutationsOfAttemptTerm);
		//If atleast one works, use that. If none work, then Rename Failure
		for (auto& permutationOfAttemptTerm : allPossiblePermutationsOfAttemptTerm) {
			if (performRenameIfValid(sourceTerm, permutationOfAttemptTerm, renamedTerm)) {
				return true;
			}
		}
		return false;
	}
	//If non-ambiguous, solve as usual.
	else {
		return performRenameIfValid(sourceTerm, attemptTerm, renamedTerm);
	}
}

bool areTensorTermsSameStructure(TensorTerm& lhs, TensorTerm& rhs) {
	for (int i = 0; i < lhs.getSize(); ++i) {
		bool sameLabel = (lhs.getTensor(i).getLabel() == rhs.getTensor(i).getLabel());
		bool sameNumOfIndices = (lhs.getTensor(i).getNumberOfIndices() == rhs.getTensor(i).getNumberOfIndices());
		bool sameNumOfBarredIndices = (lhs.getTensor(i).getNumberOfBarredIndices() == rhs.getTensor(i).getNumberOfBarredIndices());
		if (!(sameLabel && sameNumOfIndices && sameNumOfBarredIndices)) {
			return false;
		}
	}
	return true;
}

bool areTensorTermsIdentical(TensorTerm& lhs, TensorTerm& rhs) {
	if (!areTensorTermsSameStructure(lhs, rhs)) {
		return false;
	}
	//For each tensor
	for (int i = 0; i < lhs.getSize(); ++i) {
		//For each index
		for (int j = 0; j < lhs.getTensor(i).getNumberOfIndices(); ++j) {
			bool sameName = (lhs.getTensor(i).getIndexNameAt(j) == rhs.getTensor(i).getIndexNameAt(j));
			bool sameBarState = (lhs.getTensor(i).getBarStateAt(j) == rhs.getTensor(i).getBarStateAt(j));
			if (!(sameName && sameBarState)) {
				return false;
			}
		}
	}
	return true;
}

//---

MathExpression expandMathExpressions(const MathExpression& lhs, const MathExpression& rhs) {
	MathExpression resultExpression;
	resultExpression.reserveSpaceFor(lhs.getSize() * rhs.getSize());
	for (int i = 0; i < lhs.getSize(); ++i) {
		for (int j = 0; j < rhs.getSize(); ++j) {
			MathExpressionTerm lhsTerm = lhs.getTermAtLocation(i);
			MathExpressionTerm rhsTerm = rhs.getTermAtLocation(j);

			lhsTerm.mergeTerms(rhsTerm);
			resultExpression.addTerm(lhsTerm);
		}
	}
	resultExpression.simplifyCoefficientsOfAllTerms();
	return resultExpression;

}

std::string getNextName(std::string oldName) {
	char charOldName = oldName[0];
	if (charOldName == 'z') {
		charOldName = 'a';
	}
	else {
		charOldName++;
	}
	std::string result;
	result.push_back(charOldName);
	return result;

	//int num = std::stoi(oldName);
	//num++;
	//std::string result = std::to_string(num);
	//return result;
	//
}

int gcd(int a, int b) {
	if (a == 0) {
		return b;
	}
	else if (b == 0) {
		return a;
	}
	else {
		return abs(gcd(b,a%b));
	}
}

int lcm(int a, int b) {
	return ((a*b) / gcd(a, b));
}

void swapStrings(std::string& string1, std::string& string2) {
	std::string temp = string1;
	string1 = string2;
	string2 = temp;
}

bool compareIrreducibleTensorsByField(const IrreducibleTensor& lhs, const IrreducibleTensor& rhs) {
	return ((lhs.getField())<(rhs.getField()));
}

bool compareIrreducibleTensorsBySymmetryState(const IrreducibleTensor& lhs, const IrreducibleTensor& rhs) {
	return ((lhs.getSymmetryState())<(rhs.getSymmetryState()));
}

bool compareIrreducibleTensorsByBarState(const IrreducibleTensor& lhs, const IrreducibleTensor& rhs) {
	return ((lhs.getBarState())<(rhs.getBarState()));
}

bool compareIrreducibleTensorsByNumberOfLowerIndices(const IrreducibleTensor& lhs, const IrreducibleTensor& rhs) {
	return ((lhs.getNumberOfLowerIndices())<(rhs.getNumberOfLowerIndices()));
}

bool compareIrreducibleTensorsByNumberOfUpperIndices(const IrreducibleTensor& lhs, const IrreducibleTensor& rhs) {
	return ((lhs.getNumberOfUpperIndices())<(rhs.getNumberOfUpperIndices()));
}


bool checkZoneRenameIssueInIrreducibleTensors(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, const std::pair<std::string, std::string>& rename) {
	if (sourceTerm.getNumberOfLeviCivitas() == 1) {
		if ((attemptTerm.getLeviAt(0).doesIndexExist(rename.first)) && (!(sourceTerm.getLeviAt(0).doesIndexExist(rename.second)))) {
			return false;
		}
	}	
	for (int i = 0; i < attemptTerm.getNumberOfIrreducibleTensors(); ++i) {
		//if the old name exists in the attempt term but the new name does not exist in the source term.
		if ((attemptTerm.getTensorAt(i).doesIndexExistInUpperZone(rename.first)) && (!(sourceTerm.getTensorAt(i).doesIndexExistInUpperZone(rename.second)))) {
			return false;
		}
		if ((attemptTerm.getTensorAt(i).doesIndexExistInLowerZone(rename.first)) && (!(sourceTerm.getTensorAt(i).doesIndexExistInLowerZone(rename.second)))) {
			return false;
		}
	}
	return true;
}

bool performRenameIfValidOnIrreducibleTensors(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, MathExpressionTerm& renamedTerm) {
	std::vector<std::string> attemptTermIndices(attemptTerm.getListOfAllIndices());
	std::vector<std::pair<std::string, std::string>> renameMap;
	renamedTerm = attemptTerm;
	//For each attemptTerm name
	for (int i = 0; i < attemptTermIndices.size(); ++i) {
		//We are sure the index exists, and that it exists in this tensor, hence only upper/lower need be determined
		//Unless tensorOfFirstOccurence == -1, indicating it's located in the Single Levi-Civita
		int tensorOfFirstOccurence = attemptTerm.getTensorLocationOfFirstOccurenceOfIndex(attemptTermIndices[i]);
		bool isIndexInUpperZone;
		//If not found in Levi first, then check if it is in the upper or lower zone of the tensor.
		if (tensorOfFirstOccurence != -1) {
			isIndexInUpperZone = attemptTerm.getTensorAt(tensorOfFirstOccurence).doesIndexExistInUpperZone(attemptTermIndices[i]);
		}

		//If it is not in the Levi and is upper
		if ((tensorOfFirstOccurence!= -1)&&(isIndexInUpperZone)) {
			//Try to assign it to any sourceTerm index of that zone, till nothing works
			for (int j = 0; j < attemptTerm.getTensorAt(tensorOfFirstOccurence).getNumberOfUpperIndices(); ++j) {
				bool indexAlreadyAssigned = isNameAlreadyAssigned(renameMap, sourceTerm.getTensorAt(tensorOfFirstOccurence).getUpperIndexAt(j));
		
				std::pair<std::string, std::string> trialRename(attemptTermIndices[i], sourceTerm.getTensorAt(tensorOfFirstOccurence).getUpperIndexAt(j));
				bool noZoneRenameIssue = checkZoneRenameIssueInIrreducibleTensors(sourceTerm, attemptTerm, trialRename);
				
				if ((!indexAlreadyAssigned)&&(noZoneRenameIssue)) {
					renameMap.push_back(trialRename);
					break;
				}

				//If no success, and we've reached the end of the zone, Rename Failure. End process.
				if (j == (attemptTerm.getTensorAt(tensorOfFirstOccurence).getNumberOfUpperIndices() - 1)) {
					return false;
				}
			
			}
		}
		//Else if it is not in the Levi and it is lower
		else if ((tensorOfFirstOccurence != -1) && (!isIndexInUpperZone)) {
			//Try to assign it to any sourceTerm index of that zone, till nothing works
			for (int j = 0; j < attemptTerm.getTensorAt(tensorOfFirstOccurence).getNumberOfLowerIndices(); ++j) {
				bool indexAlreadyAssigned = isNameAlreadyAssigned(renameMap, sourceTerm.getTensorAt(tensorOfFirstOccurence).getLowerIndexAt(j));

				std::pair<std::string, std::string> trialRename(attemptTermIndices[i], sourceTerm.getTensorAt(tensorOfFirstOccurence).getLowerIndexAt(j));
				bool noZoneRenameIssue = checkZoneRenameIssueInIrreducibleTensors(sourceTerm, attemptTerm, trialRename);

				if ((!indexAlreadyAssigned) && (noZoneRenameIssue)) {
					renameMap.push_back(trialRename);
					break;
				}

				//If no success, and we've reached the end of the zone, Rename Failure. End process.
				if (j == (attemptTerm.getTensorAt(tensorOfFirstOccurence).getNumberOfLowerIndices() - 1)) {
					return false;
				}

			}

		}
		//Else if it is in the Single Levi
		else if (tensorOfFirstOccurence == -1) {
			//Try to assign it to any sourceTerm index of that zone, till nothing works
			for (int j = 0; j < attemptTerm.getLeviAt(0).getSize(); ++j) {
				bool indexAlreadyAssigned = isNameAlreadyAssigned(renameMap, sourceTerm.getLeviAt(0).getNameAtLocation(j));

				std::pair<std::string, std::string> trialRename(attemptTermIndices[i], sourceTerm.getLeviAt(0).getNameAtLocation(j));
				bool noZoneRenameIssue = checkZoneRenameIssueInIrreducibleTensors(sourceTerm, attemptTerm, trialRename);

				if ((!indexAlreadyAssigned) && (noZoneRenameIssue)) {
					renameMap.push_back(trialRename);
					break;
				}

				//If no success, and we've reached the end of the zone, Rename Failure. End process.
				if (j == (attemptTerm.getLeviAt(0).getSize() - 1)) {
					return false;
				}

			}


		}

	}
	renamedTerm.performRenamesOnIrreducibleTensors(renameMap);
	if (renamedTerm.getNumberOfLeviCivitas() == 1) {
		renamedTerm.performRenamesOnLeviCivita(renameMap);
	}
	return true;
}

std::string getNewNameFromRenameMap(const std::string& oldName, std::vector<std::pair<std::string, std::string>> renameMap) {
	auto it = std::find_if(renameMap.begin(), renameMap.end(), [&oldName](std::pair<std::string, std::string>& element) {return element.first == oldName; });
	if (it != renameMap.end()) {
		return it->second;
	}
	return "";
}

bool areMathExpressionTermsSameStructure(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm) {
	if (sourceTerm.getNumberOfLeviCivitas() != attemptTerm.getNumberOfLeviCivitas()) {
		return false;
	}
	if (sourceTerm.getNumberOfIrreducibleTensors() != attemptTerm.getNumberOfIrreducibleTensors()) {
		return false;
	}
	for (int i = 0; i < sourceTerm.getNumberOfIrreducibleTensors(); ++i) {
		if (!(sourceTerm.getTensorAt(i).isSameStructure(attemptTerm.getTensorAt(i)))) {
			return false;
		}
	}
	return true;
}


bool areMathExpressionTermsIdentical(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm) {
	if (!areMathExpressionTermsSameStructure(sourceTerm, attemptTerm)) {
		return false;
	}
	if (sourceTerm.getNumberOfLeviCivitas() == 1) {
		if (sourceTerm.getLeviAt(0) != attemptTerm.getLeviAt(0)) {
			return false;
		}
	}
	if (!(sourceTerm.hasSameDeltas(attemptTerm))) {
		return false;
	}
	for (int i = 0; i < sourceTerm.getNumberOfIrreducibleTensors(); ++i) {
		if (sourceTerm.getTensorAt(i) != attemptTerm.getTensorAt(i)) {
			return false;
		}
	}
	return true;
}

bool performRenameIfValidIncludingPermutationsOnIrreducibleTensors(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, MathExpressionTerm& renamedTerm) {
	//If sourceTerm (and hence also attemptTerm, since structure is same) is ambiguous
	if (sourceTerm.isAmbiguous()) {
		//Generate all possible arrangments for the attemptTerm, and try to match them with the sourceTerm
		std::vector<MathExpressionTerm> allPossiblePermutationsOfAttemptTerm;
		attemptTerm.getAllPermutationsOfAllAmbiguousZones(allPossiblePermutationsOfAttemptTerm);
		//If atleast one works, use that. If none work, then Rename Failure
		for (auto& permutationOfAttemptTerm : allPossiblePermutationsOfAttemptTerm) {
			if (performRenameIfValidOnIrreducibleTensors(sourceTerm, permutationOfAttemptTerm, renamedTerm)) {
				return true;
			}
		}
		return false;
	}
	//If non-ambiguous, solve as usual.
	else {
		return performRenameIfValidOnIrreducibleTensors(sourceTerm, attemptTerm, renamedTerm);
	}

}

bool compareDeltasByUpperIndices(const Delta& lhs, const Delta& rhs) {
	return (lhs.getUpperIndex() < rhs.getUpperIndex());
}
bool compareDeltasByLowerIndices(const Delta& lhs, const Delta& rhs) {
	return (lhs.getLowerIndex() < rhs.getLowerIndex());
}

std::vector<std::pair<std::string, std::string>> generateEmptyRenameMap() {
	std::vector<std::pair<std::string, std::string>> renameMap;
	//Generate empty rename map
	for (char charIter = 'i'; charIter <= 'r'; ++charIter) {
		std::string oldName;
		oldName.push_back(charIter);

		std::pair<std::string, std::string> tempRename(oldName, "");
		renameMap.push_back(tempRename);
	}
	return renameMap;
}


void renameDeltaExpressionBasedOnLevis(int numOfMatches, const LeviCivita& firstLevi, const LeviCivita& secondLevi, MathExpression& mathExpressionToBeRenamed) {
	//Trial
	std::vector<std::pair<std::string, std::string>> renameMap;
	if (numOfMatches == 2) {
		//From i to p
		char charIter = 'i';
		for (int i = 0; i < 5; ++i) {
			std::string oldName(1, charIter);
			if (firstLevi.getNameAtLocation(i) != oldName) {
				std::pair<std::string, std::string> tempRename(oldName, firstLevi.getNameAtLocation(i));
				renameMap.push_back(tempRename);
			}
			++charIter;
		}
		for (int i = 0; i < 3; ++i) {
			std::string oldName(1, charIter);
			if (secondLevi.getNameAtLocation(i+2) != oldName) {
				std::pair<std::string, std::string> tempRename(oldName, secondLevi.getNameAtLocation(i+2));
				renameMap.push_back(tempRename);
			}
			++charIter;
		}

	}
	else if (numOfMatches == 1) {
		//From i to q
		char charIter = 'i';
		for (int i = 0; i < 5; ++i) {
			std::string oldName(1, charIter);
			if (firstLevi.getNameAtLocation(i) != oldName) {
				std::pair<std::string, std::string> tempRename(oldName, firstLevi.getNameAtLocation(i));
				renameMap.push_back(tempRename);
			}
			++charIter;
		}
		for (int i = 0; i < 4; ++i) {
			std::string oldName(1, charIter);
			if (secondLevi.getNameAtLocation(i + 1) != oldName) {
				std::pair<std::string, std::string> tempRename(oldName, secondLevi.getNameAtLocation(i + 1));
				renameMap.push_back(tempRename);
			}
			++charIter;
		}
	}
	else if (numOfMatches == 0) {
		//From i to r
		char charIter = 'i';
		for (int i = 0; i < 5; ++i) {
			std::string oldName(1, charIter);
			if (firstLevi.getNameAtLocation(i) != oldName) {
				std::pair<std::string, std::string> tempRename(oldName, firstLevi.getNameAtLocation(i));
				renameMap.push_back(tempRename);
			}
			++charIter;
		}
		for (int i = 0; i < 5; ++i) {
			std::string oldName(1, charIter);
			if (secondLevi.getNameAtLocation(i) != oldName) {
				std::pair<std::string, std::string> tempRename(oldName, secondLevi.getNameAtLocation(i));
				renameMap.push_back(tempRename);
			}
			++charIter;
		}
	}

	mathExpressionToBeRenamed.renameDeltasOfTerms(renameMap);
}

bool isCoefficientZero(const MathExpressionTerm& MET) {
	if (MET.getCoefficient() == 0) {
		return true;
	}
	return false;
}

bool compareIndexStrengths(const std::string& lhs, const std::string& rhs) {
	//If both are in same range (before or after i) no special treatment needed
	if (((lhs >= "i")&&(rhs >= "i"))|| ((lhs < "i") && (rhs < "i"))) {
		return lhs < rhs;
	}
	//Else, on opposite sides of i, behind i is stronger
	if (lhs < "i") {
		return false;
	}
	else {
		return true;
	}
}


//**************** Phase 2 **************************************
std::string getNextNameGivenTerm(const MathExpressionTerm& met) {
	std::vector<std::string> allNames{ met.getAllUniqueNamesOfTerm() };
	if (allNames.empty()) return "i";
	std::sort(allNames.begin(), allNames.end(), compareIndexStrengths);
	for (auto iter = allNames.begin(); iter != (--(allNames.end())); ++iter) {
		std::string nextPossibleName;
		//Wrap letters to a
		if (*iter == "z") {
			nextPossibleName = "a";
		}
		else {
			nextPossibleName = std::string{ (char)((*iter)[0] + 1) };
		}
		//If gap available return this
		if (nextPossibleName != (*(iter+1))) {
			return nextPossibleName;
		}
		
	}
	//If no gap found, return next after last
	//Wrap letters to a
	if (allNames.back() == "z") {
		return "a";
	}
	else {
		return std::string{ (char)((allNames.back()[0]) + 1) };
	}
}

long combination(int n, int r) {
	return factorial(n) / (factorial(r) * factorial(n - r));
}

int reorderIndicesAntisymmetrically(std::vector<std::string>& vec) {
	int coefficient = 1;
	bool modified = true;
	while (modified) {
		modified = false;
		//Check till second last index
		for (int i = 0; i < ((int)vec.size()) - 1; ++i) {
			if ( vec[i] > vec[i+1]) {
				std::swap(vec[i],vec[i+1]);
				coefficient *= -1;
				modified = true;
			}

		}
	}
	return coefficient;
}

bool areMathExpressionTermsSameStructurePhase2(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm)
{
	if (!areMathExpressionTermsSameStructure(sourceTerm, attemptTerm)) return false;
	if (sourceTerm.getFab().getField() != attemptTerm.getFab().getField()) return false;
	if (sourceTerm.getFab().getIsNull() != attemptTerm.getFab().getIsNull()) return false;
	//sif (sourceTerm.getFab().getCharge() != attemptTerm.getFab().getCharge()) return false;
	if (sourceTerm.getNumberOfMatterTensors() != attemptTerm.getNumberOfMatterTensors()) {
		return false;
	}
	for (int i = 0; i < sourceTerm.getNumberOfMatterTensors(); ++i) {
		if (!(sourceTerm.getMatterTensorAt(i).isSameStructure(attemptTerm.getMatterTensorAt(i)))) {
			return false;
		}
	}
	return true;
}

bool areMathExpressionTermsIdenticalPhase2(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm)
{
	if (!areMathExpressionTermsIdentical(sourceTerm, attemptTerm)) return false;
	for (int i = 0; i < sourceTerm.getNumberOfMatterTensors(); ++i) {
		if (sourceTerm.getMatterTensorAt(i) != attemptTerm.getMatterTensorAt(i)) return false;
	}
	return true;
}

//NOTE: Fully ready for Phase 2
bool performRenameIfValidPhase2(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, MathExpressionTerm& renamedTerm) {
	std::vector<std::string> attemptTermIndices(attemptTerm.getAllUniqueNamesOfTerm());
	std::vector<std::pair<std::string, std::string>> renameMap;
	renamedTerm = attemptTerm;

	//For each attemptTerm name
	for (int i = 0; i < attemptTermIndices.size(); ++i) {
		LocationOfIndexPojo locationOfIndex = attemptTerm.getLocationOfFirstOccurenceOfIndex(attemptTermIndices[i]);
		bool didSucceed = true;

		if (locationOfIndex.tensorType == TensorType::IRREDUCIBLE_TENSOR) {
			if (locationOfIndex.isUpper) {
				didSucceed = trySingleRenamePhase2(sourceTerm, attemptTerm, sourceTerm.getTensorAt(locationOfIndex.loc).getUpperIndices(), renameMap, attemptTermIndices[i]);
			}
			else {
				didSucceed = trySingleRenamePhase2(sourceTerm, attemptTerm, sourceTerm.getTensorAt(locationOfIndex.loc).getLowerIndices(), renameMap, attemptTermIndices[i]);
			}
		}
		else if (locationOfIndex.tensorType == TensorType::MATTER_TENSOR) {
			if (locationOfIndex.isUpper) {
				didSucceed = trySingleRenamePhase2(sourceTerm, attemptTerm, sourceTerm.getMatterTensorAt(locationOfIndex.loc).getUpperIndices(), renameMap, attemptTermIndices[i]);
			}
			else {
				didSucceed = trySingleRenamePhase2(sourceTerm, attemptTerm, sourceTerm.getMatterTensorAt(locationOfIndex.loc).getLowerIndices(), renameMap, attemptTermIndices[i]);
			}
		}
		else if (locationOfIndex.tensorType == TensorType::LEVI) {
			didSucceed = trySingleRenamePhase2(sourceTerm, attemptTerm, sourceTerm.getLeviAt(0).getIndices(), renameMap, attemptTermIndices[i]);
		}
		else if (locationOfIndex.tensorType == TensorType::FAB) {
			didSucceed = trySingleRenamePhase2(sourceTerm, attemptTerm, sourceTerm.getFab().getIndices(), renameMap, attemptTermIndices[i]);
		}

		if (!didSucceed) return false;
	}

	renamedTerm.performRenamesPhase2(renameMap);

	return true;
}

//TODO: Make Phase 2
bool performRenameIfValidIncludingPermutationsPhase2(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, MathExpressionTerm& renamedTerm)
{
	////If sourceTerm (and hence also attemptTerm, since structure is same) is ambiguous
	//if (sourceTerm.isAmbiguous()) {
	//	//Generate all possible arrangments for the attemptTerm, and try to match them with the sourceTerm
	//	std::vector<MathExpressionTerm> allPossiblePermutationsOfAttemptTerm;
	//	attemptTerm.getAllPermutationsOfAllAmbiguousZones(allPossiblePermutationsOfAttemptTerm);
	//	//If atleast one works, use that. If none work, then Rename Failure
	//	for (auto& permutationOfAttemptTerm : allPossiblePermutationsOfAttemptTerm) {
	//		if (performRenameIfValidOnIrreducibleTensors(sourceTerm, permutationOfAttemptTerm, renamedTerm)) {
	//			return true;
	//		}
	//	}
	//	return false;
	//}
	//If non-ambiguous, solve as usual.
	//else {
	return performRenameIfValidPhase2(sourceTerm, attemptTerm, renamedTerm);
	//}
}

bool checkZoneRenameIssuePhase2(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, const std::pair<std::string, std::string>& rename){
	if (!checkZoneRenameIssueInIrreducibleTensors(sourceTerm, attemptTerm, rename)) return false;
	for (int i = 0; i < attemptTerm.getNumberOfMatterTensors(); ++i) {
		//if the old name exists in the attempt term but the new name does not exist in the source term.
		if ((attemptTerm.getMatterTensorAt(i).doesIndexExistInUpperZone(rename.first)) && (!(sourceTerm.getMatterTensorAt(i).doesIndexExistInUpperZone(rename.second)))) {
			return false;
		}
		if ((attemptTerm.getMatterTensorAt(i).doesIndexExistInLowerZone(rename.first)) && (!(sourceTerm.getMatterTensorAt(i).doesIndexExistInLowerZone(rename.second)))) {
			return false;
		}
	}
	return true;
}

bool trySingleRenamePhase2(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, const std::vector<std::string>& blindVector, std::vector<std::pair<std::string, std::string>>& renameMap, const std::string& attemptedRename){
	//Try to assign it to any sourceTerm index of that zone, till nothing works
	for (int i = 0; i < blindVector.size(); ++i) {
		bool indexAlreadyAssigned = isNameAlreadyAssigned(renameMap, blindVector[i]);

		std::pair<std::string, std::string> trialRename(attemptedRename, blindVector[i]);
		bool noZoneRenameIssue = checkZoneRenameIssuePhase2(sourceTerm, attemptTerm, trialRename);

		if ((!indexAlreadyAssigned) && (noZoneRenameIssue)) {
			renameMap.push_back(trialRename);
			break;
		}

		//If no success, and we've reached the end of the zone, Rename Failure. End process.
		if (i == (((int)blindVector.size()) - 1)) {
			return false;
		}
	}
	return true;
}


