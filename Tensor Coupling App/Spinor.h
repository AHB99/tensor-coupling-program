#pragma once

#include <string>
#include <iostream>

using std::cout;
using std::cin;
using std::endl;

class Spinor {
public:
	void print() const;
	void inputByUser();
	void setIsLeft(bool inpIsLeft) {
		isLeft = inpIsLeft;
	}
	void setIsStarred(bool inpIsStarred) {
		isStarred = inpIsStarred;
	}
	bool getIsLeft() const {
		return isLeft;
	}
	std::string getIndex() const {
		return index;
	}
	bool getIsStarred() const {
		return isStarred;
	}
private:
	std::string index;
	bool isLeft;
	bool isStarred;
};
