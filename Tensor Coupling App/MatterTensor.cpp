#include "MatterTensor.h"

void MatterTensor::print() const {
	cout << "M";
	if (isTranspose) {
		cout << "(T)";
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
}

void MatterTensor::inputByUser() {
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

	cout << "Transpose? (0 = No, 1 = Yes): ";
	cin >> isTranspose;
}

bool MatterTensor::replaceIndexIfPresent(std::string oldIndex, std::string newIndex) {
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
