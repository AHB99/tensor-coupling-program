#include "NormalizedTensor.h"
#include "IrreducibleTensor.h"
#include "UtilityFunctions.h"

NormalizedTensor::NormalizedTensor(bool inpIsSymmetric, int inpField) : isSymmetric(inpIsSymmetric), field(inpField) { }

void NormalizedTensor::print() const {
	cout << "H";
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

void NormalizedTensor::printLatex() const {
	if (isBarred) {
		cout << "\\bar{H}";
	}
	else {
		cout << "H";
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

std::string NormalizedTensor::getPhysicalFieldFromEncodedField() const {
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


void NormalizedTensor::addUpperIndex(const std::string& index) {
	upperIndices.push_back(index);
}

void NormalizedTensor::addLowerIndex(const std::string& index) {
	lowerIndices.push_back(index);
}

void NormalizedTensor::setIndicesFromIrreducibleTensor(const IrreducibleTensor& irreducibleTensor) {
	for (int i = 0; i < irreducibleTensor.getNumberOfUpperIndices(); ++i) {
		upperIndices.push_back(irreducibleTensor.getUpperIndexAt(i));
	}
	for (int i = 0; i < irreducibleTensor.getNumberOfLowerIndices(); ++i) {
		lowerIndices.push_back(irreducibleTensor.getLowerIndexAt(i));
	}
}
