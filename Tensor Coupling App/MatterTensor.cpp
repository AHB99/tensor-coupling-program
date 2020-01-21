#include "MatterTensor.h"
#include "UtilityFunctions.h"
#include <algorithm>

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

void MatterTensor::printLatex() const {
	cout << "M";

	if (!upperIndices.empty()) {
		cout << "^{";
		for (auto& index : upperIndices) {
			cout << index;
		}
		if (isTranspose) { cout << " T";}
		cout << "}";
	}
	else if (isTranspose) {cout << "^{T}";}
	
	if (!lowerIndices.empty()) {
		cout << "_{";
		for (auto& index : lowerIndices) {
			cout << index;
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

int MatterTensor::reorderIndices(){
	return reorderIndicesAntisymmetrically(upperIndices) * reorderIndicesAntisymmetrically(lowerIndices);
}

//Ignores Transpose
bool MatterTensor::isSameStructure(const MatterTensor& rhs) const{
	if (upperIndices.size() != rhs.upperIndices.size()) return false;
	if (lowerIndices.size() != rhs.lowerIndices.size()) return false;
	return true;
}

bool MatterTensor::operator==(const MatterTensor& rhs) const {
	if (!isSameStructure(rhs)) return false;
	for (int i = 0; i < upperIndices.size(); ++i) { if (upperIndices[i] != rhs.upperIndices[i]) return false; }
	for (int i = 0; i < lowerIndices.size(); ++i) { if (lowerIndices[i] != rhs.lowerIndices[i]) return false; }
	return true;
}

bool MatterTensor::operator!=(const MatterTensor& rhs) const{
	return !(operator==(rhs));
}

bool MatterTensor::doesIndexExistInUpperZone(const std::string& index) const{
	return (std::find(upperIndices.begin(), upperIndices.end(), index) != upperIndices.end());
}

bool MatterTensor::doesIndexExistInLowerZone(const std::string& index) const{
	return (std::find(lowerIndices.begin(), lowerIndices.end(), index) != lowerIndices.end());
}

void MatterTensor::changeUpperIndexAtLoc(int loc, const std::string& newName) {
	upperIndices[loc] = newName;
}

void MatterTensor::changeLowerIndexAtLoc(int loc, const std::string& newName) {
	lowerIndices[loc] = newName;
}
