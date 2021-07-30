#include "Tensor.h"
#include "UtilityFunctions.h"
// TENSOR MEMBER DEFS


void Tensor::addIndex(std::string name, int barstatus) {
	indices.push_back(std::make_pair(name, barstatus));
}

int Tensor::getNumberOfIndices() const {
	return indices.size();
}

void Tensor::printTensor() {
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

void Tensor::printTensorAsLatex() {
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

void Tensor::swapIndexWithNextOne(int location) {
	std::string tempName;
	int tempBarstate;
	tempName = indices[location].first;
	tempBarstate = indices[location].second;
	indices[location].first = indices[location + 1].first;
	indices[location].second = indices[location + 1].second;
	indices[location + 1].first = tempName;
	indices[location + 1].second = tempBarstate;
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

