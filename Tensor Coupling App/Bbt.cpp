#include "Bbt.h"

void Bbt::print() const {
	cout << "b";
	if (dagger) { cout << "t"; }
	cout << "{" << index << "}";
}

void Bbt::inputByUser() {
	cout << "Index : ";
	cin >> index;
	cout << "Dagger? (0 = No, 1 = Yes): ";
	cin >> dagger;
}
