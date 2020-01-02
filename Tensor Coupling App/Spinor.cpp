#include "Spinor.h"

void Spinor::print() const {
	if (isLeft) {
		cout << "<";
	}
	else {
		cout << "|";
	}
	cout << "Y";
	if (isStarred) {
		cout << "*";
	}		
	cout << "_{" << index << "}";
	if (isLeft) {
		cout << "|";
	}
	else {
		cout << ">";
	}
}

void Spinor::printLatex() const {
	if (isLeft) {
		cout << "<";
	}
	else {
		cout << "|";
	}
	cout << "\\Psi";
	if (isStarred) {
		cout << "^{*}";
	}
	cout << "_{" << index << "}";
	if (isLeft) {
		cout << "|";
	}
	else {
		cout << ">";
	}
}

void Spinor::inputByUser() {
	cout << "Enter index: ";
	cin >> index;
	cout << "Is Starred? (0 = No, 1 = Yes): ";
	cin >> isStarred;
}
