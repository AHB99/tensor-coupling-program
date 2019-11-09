#include "Fab.h"
#include "UtilityFunctions.h"
#include <algorithm>


void Fab::print() const {
	cout << "F{";
	for (auto& index : indices) {
		cout << index << " ";
	}
	cout << "}{" << field << "}";
	if (charge != FabCharge::UNCHARGED) {
		if (charge == FabCharge::POSITIVE) {
			cout << "{+}";
		}
		else {
			cout << "{-}";
		}
	}
}

void Fab::inputByUser() {
	cout << "Enter fab indices till /: ";
	std::string tempString;
	cin >> tempString;
	while (tempString != "/") {
		indices.push_back(tempString);
		cin >> tempString;
	}
	cout << "Field: ";
	cin >> field;
	cout << "Charge? (0 = Uncharged, 1 = Positive, 2 = Negative): ";
	int tempNum;
	cin >> tempNum;
	if (tempNum == 0) {
		charge = FabCharge::UNCHARGED;
	}
	else if (tempNum == 1) {
		charge = FabCharge::POSITIVE;
	}
	else if (tempNum == 2) {
		charge = FabCharge::NEGATIVE;
	}
}

int Fab::reorderIndices() {
	int antisymmetricCoefficient = reorderIndicesAntisymmetrically(indices);
	//Assuming symmetric
	return 1;
}

bool Fab::doesIndexExist(const std::string& index) const {
	return (std::find(indices.begin(), indices.end(), index) != indices.end());
}
