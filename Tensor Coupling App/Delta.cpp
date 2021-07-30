#include "Delta.h"
#include "IrreducibleTensor.h"

void Delta::print() const {
	cout << "d{" << upperIndex << " " << lowerIndex << "}";
}

void Delta::printLatex() const {
	cout << "\\delta^{" << upperIndex << "}_{" << lowerIndex << "}";
}


void Delta::inputByUser() {
	std::string tempString;
	cout << "Upper & Lower index: ";
	cin >> tempString;
	upperIndex = tempString;
	cin >> tempString;
	lowerIndex = tempString;
}

bool Delta::sumIndicesIfPossible(const Delta& otherDelta) {
	bool sourceDeltaUpperMatch = false, sourceDeltaLowerMatch = false;
	std::string otherIndex;

	if (upperIndex == otherDelta.upperIndex) {
		sourceDeltaUpperMatch = true;
		otherIndex = otherDelta.lowerIndex;
	}
	else if (upperIndex == otherDelta.lowerIndex) {
		sourceDeltaUpperMatch = true;
		otherIndex = otherDelta.upperIndex;
	}

	if (lowerIndex == otherDelta.upperIndex) {
		if (sourceDeltaUpperMatch) {
			return false;
		}
		otherIndex = otherDelta.lowerIndex;
		sourceDeltaLowerMatch = true;
	}
	else if (lowerIndex == otherDelta.lowerIndex) {
		if (sourceDeltaUpperMatch) {
			return false;
		}
		otherIndex = otherDelta.upperIndex;
		sourceDeltaLowerMatch = true;
	}

	if (sourceDeltaUpperMatch) {
		upperIndex = otherIndex;
		return true;
	}
	else if (sourceDeltaLowerMatch) {
		lowerIndex = otherIndex;
		return true;
	}
	else {
		return false;
	}
}

bool Delta::isCancellationDeltaForTensor(const IrreducibleTensor& tensor) const {
	if (tensor.doesIndexExist(upperIndex) && tensor.doesIndexExist(lowerIndex)) {
		return true;
	}
	return false;
}

bool Delta::replaceIndexIfPossible(IrreducibleTensor& tensor) const {
	//Assumes that only 1 match is possible
	return (tensor.replaceIndexIfPresent(upperIndex, lowerIndex) || tensor.replaceIndexIfPresent(lowerIndex, upperIndex));
}

bool Delta::isIdentical(const Delta& otherDelta) const {
	bool sourceDeltaUpperMatch = (upperIndex == otherDelta.upperIndex) || (upperIndex == otherDelta.lowerIndex);
	bool sourceDeltaLowerMatch = (lowerIndex == otherDelta.upperIndex) || (lowerIndex == otherDelta.lowerIndex);
	return (sourceDeltaUpperMatch && sourceDeltaLowerMatch);
}

Delta::Delta(const std::string& inpUpperIndex, const std::string& inpLowerIndex) {
	upperIndex = inpUpperIndex;
	lowerIndex = inpLowerIndex;
}

void Delta::setIndices(const std::string& inpUpperIndex, const std::string& inpLowerIndex) {
	upperIndex = inpUpperIndex;
	lowerIndex = inpLowerIndex;
}

void Delta::setUpperIndex(const std::string& inpUpperIndex) {
	upperIndex = inpUpperIndex;
}

void Delta::setLowerIndex(const std::string& inpLowerIndex) {
	lowerIndex = inpLowerIndex;
}


