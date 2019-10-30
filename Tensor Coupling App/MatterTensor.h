#pragma once
	
#include <vector>
#include <string>
#include <iostream>

using std::cout;
using std::cin;
using std::endl;


class MatterTensor {
public:
	void print() const;
	void inputByUser();
	void addUpperIndex(const std::string& index) {
		upperIndices.push_back(index);
	}
	void addLowerIndex(const std::string& index) {
		lowerIndices.push_back(index);
	}
	void setIsTranspose(bool inpTranspose) {
		isTranspose = inpTranspose;
	}

	std::vector<std::string> getIndices() const {
		std::vector<std::string> result;
		result.insert(result.begin(), upperIndices.begin(), upperIndices.end());
		result.insert(result.begin(), lowerIndices.begin(), lowerIndices.end());
		return result;
	}
private:
	std::vector<std::string> upperIndices;
	std::vector<std::string> lowerIndices;
	bool isTranspose;
};