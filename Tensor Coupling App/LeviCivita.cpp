#include "LeviCivita.h"
#include "UtilityFunctions.h"

LeviCivita::LeviCivita(bool inpIsUpper) : isUpper(inpIsUpper) { }


void LeviCivita::print() const {
	cout << "E";
	if (isUpper) {
		cout << "^{";
	}
	else {
		cout << "_{";
	}
	for (auto& index : indices) {
		cout << index << " ";
	}
	cout << "}";
}

void LeviCivita::printLatex() const {
	cout << "\\epsilon";
	if (isUpper) {
		cout << "^{";
	}
	else {
		cout << "_{";
	}
	for (auto& index : indices) {
		cout << index;
	}
	cout << "}";
}



void LeviCivita::inputByUser() {
	cout << "Enter 5 indices: ";

	for (int i = 0; i < 5; ++i) {
		std::string tempString;
		cin >> tempString;
		indices.push_back(tempString);
	}
}

void LeviCivita::addIndex(const std::string& inpIndex) {
	indices.push_back(inpIndex);
}

std::string LeviCivita::getNameAtLocation(int location) const {
	return indices[location];
}

void LeviCivita::swapIndexAtLocationWithOneOnLeft(int location) {
	std::string temp = indices[location];
	indices[location] = indices[location - 1];
	indices[location - 1] = temp;
}

int LeviCivita::getLocationOfIndex(std::string indexName) const {
	for (int i = 0; i < indices.size(); ++i) {
		if (indices[i] == indexName) {
			return i;
		}
	}
	return -1;
}

bool LeviCivita::doesIndexExist(const std::string& index) const {
	if (std::find(indices.begin(), indices.end(), index) == indices.end()) {
		return false;
	}
	return true;
}

void LeviCivita::changeIndexAtLocationTo(int location, const std::string& newName) {
	indices[location] = newName;
}

void LeviCivita::swapIndexAtLocationWithOneOnRight(int location) {
	swapStrings(indices[location], indices[location + 1]);
}

bool LeviCivita::operator==(const LeviCivita& rhs) const {
	for (int i = 0; i < indices.size(); ++i) {
		if (indices[i] != rhs.getNameAtLocation(i)) {
			return false;
		}
	}
	return true;
}

bool LeviCivita::operator!=(const LeviCivita& rhs) const {
	return (!operator==(rhs));
}

int LeviCivita::numberOfSimilarIndices(const LeviCivita& otherLevi) const {
	int result = 0;
	for (auto& index : indices) {
		for (auto& otherIndex : otherLevi.indices) {
			if (index == otherIndex) {
				++result;
				break;
			}
		}
	}
	return result;
}
