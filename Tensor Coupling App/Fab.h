#pragma once
#include <iostream>
#include <vector>
#include <string>

using std::cout;
using std::cin;
using std::endl;

enum class FabCharge {
	UNCHARGED,
	POSITIVE,
	NEGATIVE
};

class Fab {
public:
	void print() const;
	void printLatex() const;

	void inputByUser();
	const std::vector<std::string>& getIndices() const {
		return indices;
	}
	bool getIsNull() const {
		return isNull;
	}
	FabCharge getCharge() const {
		return charge;
	}
	void setCharge(FabCharge inpCharge) {
		charge = inpCharge;
	}
	int getField() const {
		return field;
	}
	
	int getNumberOfIndices() const {
		return indices.size();
	}
	const std::string& getIndexAt(int loc) const {
		return indices[loc];
	}
	int reorderIndices();
	bool doesIndexExist(const std::string& index) const;
	void changeIndexAt(int loc, const std::string& index) {
		indices[loc] = index;
	}


private:
	bool isNull = false;
	int field;
	std::vector<std::string> indices;
	FabCharge charge;
};

