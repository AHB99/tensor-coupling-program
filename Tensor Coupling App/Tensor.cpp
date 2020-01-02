#include "Tensor.h"
#include "UtilityFunctions.h"
#include "BbtChain.h"
#include <map>
#include <set>
// TENSOR MEMBER DEFS


void Tensor::addIndex(std::string name, int barstatus) {
	indices.push_back(std::make_pair(name, barstatus));
}

int Tensor::getNumberOfIndices() const {
	return indices.size();
}

void Tensor::printTensor() const {
	cout << label;
	cout << "<";
	for (int i = 0; i < indices.size(); ++i) {
		cout << indices[i].first;
		if (indices[i].second == 1) {
			cout << "b";
		}
		cout << " ";
	}
	cout << ">";
}

void Tensor::printLatex() const {
	cout << label << "_{";
	for (int i = 0; i < indices.size(); ++i) {
		if (indices[i].second == 1) {
			cout << ("\\bar{c" + indices[i].first + "}");
		}
		else {
			cout << ("c" + indices[i].first);
		}
	}
	cout << "}";
}



void Tensor::setLabel(std::string inpLabel) {
	label = inpLabel;
}

int Tensor::getUnbarredZoneLocation() {
	//If the very last is barred, there are no unbarred.
	if (indices[indices.size() - 1].second == 1) {
		return -1;
	}
	for (int i = indices.size() - 1; i >= 0; --i) {
		if (indices[i].second == 1) {
			return (i + 1);
		}
		if (i == 0) {
			return i;
		}
	}
	return -1;
}

int Tensor::getBarredZoneLocation() {
	//If the very first is unbarred, there are no barred.
	if (indices[0].second == 0) {
		return -1;
	}
	for (int i = 0; i < indices.size(); ++i) {
		if (indices[i].second == 0) {
			return (i - 1);
		}
		if (i == indices.size() - 1) {
			return i;
		}
	}
	return -1;
}

bool Tensor::doesNameExistInUnbarredZone(std::string newName) {
	for (int i = getUnbarredZoneLocation(); i < indices.size(); ++i) {
		if (indices[i].first == newName) {
			return true;
		}
	}
	return false;
}

bool Tensor::doesNameExistInBarredZone(std::string newName) {
	for (int i = 0; i <= getBarredZoneLocation(); ++i) {
		if (indices[i].first == newName) {
			return true;
		}
	}
	return false;
}

int Tensor::getBarStateFromName(std::string name) {
	for (auto& index : indices) {
		if (index.first == name) {
			return index.second;
		}
	}
	return -1;
}

bool Tensor::doesNameExist(std::string name) {
	for (auto& index : indices) {
		if (index.first == name) {
			return true;
		}
	}
	return false;
}

void Tensor::changeIndexNameTo(int location, std::string newName) {
	indices[location].first = newName;
}

void Tensor::swapIndexWithOneOnRight(int location) {
	std::string tempName;
	int tempBarstate;
	tempName = indices[location].first;
	tempBarstate = indices[location].second;
	indices[location].first = indices[location + 1].first;
	indices[location].second = indices[location + 1].second;
	indices[location + 1].first = tempName;
	indices[location + 1].second = tempBarstate;
}

void Tensor::swapIndexWithOneOnLeft(int location) {
	if (location == 0) return;
	std::swap(indices[location], indices[location - 1]);
}

int Tensor::getNumberOfBarredIndices() const {
	int result = 0;
	for (auto& index : indices) {
		if (index.second == 1) {
			++result;
		}
	}
	return result;
}

//*************************Phase 2********************
bool Tensor::replaceIndexIfPresent(const std::string& oldIndex, const std::string& newIndex) {
	for (auto& index : indices) {
		if (index.first == oldIndex) {
			index.first = newIndex;
			return true;
		}
	}
	return false;
}

BbtChain Tensor::convertToBbtChain() {
	BbtChain resultBbtChain;
	for (auto& index : indices) {
		if (index.second == 0) {
			resultBbtChain.addBbt(index.first, false);
		}
		else {
			resultBbtChain.addBbt(index.first, true);
		}
	}

	return resultBbtChain;
}

int Tensor::getNumberOfDuplicatedIndices() const {
	return getDuplicatedIndices().size();
}

std::vector<std::string> Tensor::getDuplicatedIndices() const {
	std::vector<std::string> result;
	std::set<std::string> nameSet;
	for (auto& index : indices) {
		if (nameSet.count(index.first) == 0) {
			nameSet.insert(index.first);
		}
		else {
			result.push_back(index.first);
		}
	}
	return result;
}

//
//std::vector<std::string> Tensor::getNonDuplicatedBarredIndices() const {
//	std::vector<std::string> result;
//	std::map<std::string, int> nameCounterMap;
//	for (auto& index : indices) {
//		if (nameCounterMap.count(index.first) == 0) {
//			nameCounterMap[index.first] = 1;
//		}
//		else {
//			++nameCounterMap[index.first];
//		}
//	}
//	for (auto& index : indices) {
//		if (nameCounterMap[index.first] < 2 && index.second == 1) {
//			result.push_back(index.first);
//		}
//	}
//	return result;
//}
//std::vector<std::string> Tensor::getNonDuplicatedUnbarredIndices() const {
//	std::vector<std::string> result;
//	std::map<std::string, int> nameCounterMap;
//	for (auto& index : indices) {
//		if (nameCounterMap.count(index.first) == 0) {
//			nameCounterMap[index.first] = 1;
//		}
//		else {
//			++nameCounterMap[index.first];
//		}
//	}
//	for (auto& index : indices) {
//		if (nameCounterMap[index.first] < 2 && index.second == 0) {
//			result.push_back(index.first);
//		}
//	}
//	return result;
//}

int Tensor::getLocationOfFirstOccurenceOfIndex(const std::string& indexName) const {
	for (int i = 0; i < indices.size(); ++i) {
		if (indices[i].first == indexName) {
			return i;
		}
	}
	return -1;
}

int Tensor::getLocationOfSecondOccurenceOfIndex(const std::string& indexName) const {
	for (int i = getLocationOfFirstOccurenceOfIndex(indexName) + 1; i < indices.size(); ++i) {
		if (indices[i].first == indexName) {
			return i;
		}
	}
	return -1;
}

