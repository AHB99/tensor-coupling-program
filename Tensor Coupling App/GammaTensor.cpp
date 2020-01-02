#include "GammaTensor.h"

void GammaTensor::print() const {
	if (isNull) return;
	cout << "G_{";
	for (auto& index : indices) {
		cout << index;
	}
	cout << "}";
}

void GammaTensor::printLatex() const {
	if (isNull) return;
	cout << "\\Gamma_{";
	for (auto& index : indices) {
		cout << index;
	}
	cout << "}";
}

void GammaTensor::inputByUser() {
	cout << "Gamma Null? (0 = no, 1 = yes): ";
	cin >> isNull;
	if (!isNull) {
		cout << "Enter Gamma indices till /: ";
		std::string tempString;
		cin >> tempString;
		while (tempString != "/") {
			indices.push_back(tempString);
			cin >> tempString;
		}
	}
}