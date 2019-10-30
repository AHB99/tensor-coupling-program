#pragma once
#include <iostream>
#include <vector>
#include <string>

using std::cout;
using std::cin;
using std::endl;

enum FabCharge {
	Uncharged,
	Positive,
	Negative
};

class Fab {
public:
	void print() const ;
	void inputByUser();
	std::vector<std::string> getIndices() const {
		return indices;
	}

private:
	int field;
	std::vector<std::string> indices;
	FabCharge charge;
};

