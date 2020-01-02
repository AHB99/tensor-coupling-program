#pragma once

#include <vector>
#include <string>
#include <iostream>

using std::cout;
using std::cin;
using std::endl;

class GammaTensor {
public:
	void print() const;
	void printLatex() const;

	void inputByUser();

	std::vector<std::string> getIndices() const {
		return indices;
	}

	void setIsNull(bool inpIsNull) {
		isNull = inpIsNull;
	}

private:
	std::vector<std::string> indices;
	bool isNull;
};
