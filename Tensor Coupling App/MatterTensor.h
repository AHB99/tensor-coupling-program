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

	int getNumberOfUpperIndices() const {
		return upperIndices.size();
	}

	int getNumberOfLowerIndices() const {
		return lowerIndices.size();
	}

	bool replaceIndexIfPresent(std::string oldIndex, std::string newIndex);

	int reorderIndices();
	bool isSameStructure(const MatterTensor& rhs) const;
	bool operator==(const MatterTensor& rhs) const;
	bool operator!=(const MatterTensor& rhs) const;
	bool doesIndexExistInUpperZone(const std::string& index) const;
	bool doesIndexExistInLowerZone(const std::string& index) const;
	bool doesIndexExist(const std::string& index) const {
		return (doesIndexExistInUpperZone(index) || doesIndexExistInLowerZone(index));
	}

	const std::vector<std::string>& getUpperIndices() const {
		return upperIndices;
	}
	const std::vector<std::string>& getLowerIndices() const {
		return lowerIndices;
	}

	std::string getUpperIndexAt(int loc) const {
		return upperIndices[loc];
	}
	std::string getLowerIndexAt(int loc) const {
		return lowerIndices[loc];
	}


	void changeUpperIndexAtLoc(int loc, const std::string& newName);
	void changeLowerIndexAtLoc(int loc, const std::string& newName);


private:
	std::vector<std::string> upperIndices;
	std::vector<std::string> lowerIndices;
	bool isTranspose;
};