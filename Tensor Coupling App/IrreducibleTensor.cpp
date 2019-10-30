#include "IrreducibleTensor.h"
#include "UtilityFunctions.h"

IrreducibleTensor::IrreducibleTensor(bool inpIsSymmetric, int inpField) : isSymmetric(inpIsSymmetric), field(inpField) { }


void IrreducibleTensor::print() const {
	cout << "h";
	if (isBarred) {
		cout << "b";
	}
	if (!upperIndices.empty()) {
		cout << "^{";
		for (auto& index : upperIndices) {
			cout << index << " ";
		}
		cout << "}";
	}
	if (!lowerIndices.empty()) {
		cout << "_{";
		for (auto& index : lowerIndices) {
			cout << index << " ";
		}
		cout << "}";
	}
	cout << isSymmetric << "," << field << "|";
}

void IrreducibleTensor::printLatex() const {
	if (isBarred) {
		cout << "\\bar{h}";
	}
	else {
		cout << "h";
	}
	if (!upperIndices.empty()) {
		cout << "^{";
		for (auto& index : upperIndices) {
			cout << index;
		}
		cout << " (" << getPhysicalFieldFromEncodedField() << ")";
		cout << "}";
	}
	else {
		cout << "^{(" << getPhysicalFieldFromEncodedField() << ")}";
	}


	if (!lowerIndices.empty()) {
		cout << "_{";
		for (auto& index : lowerIndices) {
			cout << index;
		}
		if (isSymmetric) {
			cout << " (S)";
		}
		cout << "}";
	}
	else {
		if (isSymmetric) {
			cout << "_{(S)}";
		}
	}
	
}


void IrreducibleTensor::inputByUser() {
	int tempNum;
	std::string tempString;
	cout << "Upper indices till '/': ";
	cin >> tempString;
	while (tempString != "/") {
		upperIndices.push_back(tempString);
		cin >> tempString;
	}
	cout << "Lower indices till '/': ";
	cin >> tempString;
	while (tempString != "/") {
		lowerIndices.push_back(tempString);
		cin >> tempString;
	}

	cout << "Symmetric? (0 = No, 1 = Yes): ";
	cin >> tempNum;
	isSymmetric = tempNum;
	cout << "Field: ";
	cin >> tempNum;
	field = tempNum;

}

bool IrreducibleTensor::doesIndexExist(std::string index) const {
	return (doesIndexExistInUpperZone(index)||doesIndexExistInLowerZone(index));
}

bool IrreducibleTensor::doesIndexExistInUpperZone(std::string index) const {
	for (auto& upperIndex : upperIndices) {
		if (upperIndex == index) {
			return true;
		}
	}
	return false;
}
bool IrreducibleTensor::doesIndexExistInLowerZone(std::string index) const {
	for (auto& lowerIndex : lowerIndices) {
		if (lowerIndex == index) {
			return true;
		}
	}
	return false;
}



bool IrreducibleTensor::replaceIndexIfPresent(std::string oldIndex, std::string newIndex) {
	for (auto& upperIndex : upperIndices) {
		if (upperIndex == oldIndex) {
			upperIndex = newIndex;
			return true;
		}
	}
	for (auto& lowerIndex : lowerIndices) {
		if (lowerIndex == oldIndex) {
			lowerIndex = newIndex;
			return true;
		}
	}
	return false;
}

void IrreducibleTensor::addUpperIndex(const std::string& index) {
	upperIndices.push_back(index);
}

void IrreducibleTensor::addLowerIndex(const std::string& index) {
	lowerIndices.push_back(index);
}

void IrreducibleTensor::swapUpperIndexAtLocationWithOneOnRight(int location) {
	swapStrings(upperIndices[location], upperIndices[location + 1]);
}

void IrreducibleTensor::swapLowerIndexAtLocationWithOneOnRight(int location) {
	swapStrings(lowerIndices[location], lowerIndices[location + 1]);
}

std::string IrreducibleTensor::getUpperIndexAt(int location) const {
	return upperIndices[location];
}

std::string IrreducibleTensor::getLowerIndexAt(int location) const {
	return lowerIndices[location];
}

void IrreducibleTensor::changeUpperIndexAtLocationTo(int location, const std::string& newName) {
	upperIndices[location] = newName;
}

void IrreducibleTensor::changeLowerIndexAtLocationTo(int location, const std::string& newName) {
	lowerIndices[location] = newName;
}

bool IrreducibleTensor::operator==(const IrreducibleTensor& rhs) const {
	//Will never be identical if not atleast same structure
	if (!isSameStructure(rhs)) {
		return false;
	}
	for (int i = 0; i < upperIndices.size(); ++i) {
		if (upperIndices[i] != rhs.upperIndices[i]) {
			return false;
		}
	}
	for (int i = 0; i < lowerIndices.size(); ++i) {
		if (lowerIndices[i] != rhs.lowerIndices[i]) {
			return false;
		}
	}
	return true;
}

bool IrreducibleTensor::operator!=(const IrreducibleTensor& rhs) const {
	return (!(operator==(rhs)));
}

bool IrreducibleTensor::isSameStructure(const IrreducibleTensor& otherIrreducibleTensor) const {
	if ((isBarred != otherIrreducibleTensor.isBarred) || (field != otherIrreducibleTensor.field) || (isSymmetric != otherIrreducibleTensor.isSymmetric)) {
		return false;
	}
	if (upperIndices.size() != otherIrreducibleTensor.upperIndices.size()) {
		return false;
	}
	if (lowerIndices.size() != otherIrreducibleTensor.lowerIndices.size()) {
		return false;
	}
	return true;
}

bool IrreducibleTensor::hasAtleast2MatchingIndices(const IrreducibleTensor& otherIrreducibleTensor) const {
	//Check upper indices
	int matches = 0;
	for (auto& sourceUpperIndex : upperIndices) {
		if (otherIrreducibleTensor.doesIndexExist(sourceUpperIndex)) {
			++matches;
		}
	}
	for (auto& sourceLowerIndex : lowerIndices) {
		if (otherIrreducibleTensor.doesIndexExist(sourceLowerIndex)) {
			++matches;
		}
	}
	if (matches >= 2) {
		return true;
	}
	else {
		return false;
	}
}

std::string IrreducibleTensor::getPhysicalFieldFromEncodedField() const {
	if (field == 1) {
		return "10";
	}
	else if (field == 2) {
		return "45";
	}
	else if (field == 3) {
		return "120";
	}
	else if (field == 4) {
		return "210";
	}
	else if (field == 5) {
		if (isBarred) {
			return "126";
		}
		else if ((upperIndices.size() == 0) && (lowerIndices.size() == 1)) {
			return "126";
		}
		else if ((upperIndices.size() == 2) && (lowerIndices.size() == 0) && (!isSymmetric)) {
			return "126";
		}
		else if ((upperIndices.size() == 0) && (lowerIndices.size() == 2) && (isSymmetric)) {
			return "126";
		}
		else if ((upperIndices.size() == 2) && (lowerIndices.size() == 1)) {
			return "126";
		}
		else if ((upperIndices.size() == 2) && (lowerIndices.size() == 3)) {
			return "126";
		}
		else {
			return "\\bar{126}";
		}
	}
}