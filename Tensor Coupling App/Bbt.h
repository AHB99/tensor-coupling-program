#pragma once
#include <string>
#include <iostream>

using std::cout;
using std::cin;
using std::endl;

class Bbt {
public:
	Bbt() = default;

	Bbt(std::string inpName, bool inpDagger) {
		index = inpName;
		dagger = inpDagger;
	}
	void print() const;
	void printLatex() const;

	void inputByUser() ;
	std::string getName() const {
		return index;
	}
	bool isDagger() const {
		return dagger;
	}
	void setDagger(bool inpDagger) {
		dagger = inpDagger;
	}

private:
	std::string index;
	bool dagger;
};