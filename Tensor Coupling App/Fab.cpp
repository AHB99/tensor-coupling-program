#include "Fab.h"


void Fab::print() const {
	cout << "F{";
	for (auto& index : indices) {
		cout << index << " ";
	}
	cout << "}{" << field << "}";
	if (charge != Uncharged) {
		if (charge == Positive) {
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
		charge = Uncharged;
	}
	else if (tempNum == 1) {
		charge = Positive;
	}
	else if (tempNum == 2) {
		charge = Negative;
	}
}
