#include "ProductResolver.hpp"
#include "TensorTerm.h"
#include "Tensor.h"
#include "UtilityFunctions.h"
#include <cmath>
#include <algorithm>
#include <regex>

ProductResolver::ProductResolver() {
	LeviCivita tempLevi2MatchFirst, tempLevi2MatchSecond, tempLevi1MatchFirst, tempLevi1MatchSecond, tempLevi0MatchFirst, tempLevi0MatchSecond;
	//Determine 2 match expression
	std::vector<std::string> tempVec2MatchFirst = { "i", "j", "k", "l", "m" };
	std::vector<std::string> tempVec2MatchSecond = { "i", "j", "n", "o", "p" };
	for (auto& index : tempVec2MatchFirst) {
		tempLevi2MatchFirst.addIndex(index);
	}
	for (auto& index : tempVec2MatchSecond) {
		tempLevi2MatchSecond.addIndex(index);
	}
	predetermined2Match.setPredeterminedSubstitutionsFor2Matches(tempLevi2MatchFirst, tempLevi2MatchSecond);

	//Determine 1 match expression
	std::vector<std::string> tempVec1MatchFirst = { "i", "j", "k", "l", "m" };
	std::vector<std::string> tempVec1MatchSecond = { "i", "n", "o", "p", "q" };
	for (auto& index : tempVec1MatchFirst) {
		tempLevi1MatchFirst.addIndex(index);
	}
	for (auto& index : tempVec1MatchSecond) {
		tempLevi1MatchSecond.addIndex(index);
	}
	predetermined1Match.setPredeterminedSubstitutionsFor1Match(tempLevi1MatchFirst, tempLevi1MatchSecond);


	//Determine 0 match expression

	std::vector<std::string> tempVec0MatchFirst = { "i", "j", "k", "l", "m" };
	std::vector<std::string> tempVec0MatchSecond = { "n", "o", "p", "q", "r" };
	for (auto& index : tempVec0MatchFirst) {
		tempLevi0MatchFirst.addIndex(index);
	}
	for (auto& index : tempVec0MatchSecond) {
		tempLevi0MatchSecond.addIndex(index);
	}
	predetermined0Match.setPredeterminedSubstitutionsFor0Matches(tempLevi0MatchFirst, tempLevi0MatchSecond);
}


void ProductResolver::printSimplifiedReducibleTensorTermsAsLatex() {

	std::cout << "\n";
	int trivialDenominator = pow(2, allIndexNames.size());

	if (trivialDenominator != 1) {
		std::cout << "\\frac{1}{" + std::to_string(trivialDenominator) + "}(";
	}

	for (int i = 0; i < simplifiedTensorTerms.size(); ++i) {
		simplifiedTensorTerms[i].printTensorTermAsLatex();
	}

	if (trivialDenominator != 1) {
		cout << ")";
	}
	cout << "\n";
}


void ProductResolver::generateAllUnsimplifiedReducibleTensorTerms() {

	// We perform all permutations using binary counting. Start with calculating the end point...
	// Number of bits in the end = number of subscripts.

	unsigned int numberOfIndices = allIndexNames.size();
	uint32_t end = 1 << numberOfIndices;
	for (uint32_t i = 0; i < end; i++) {

		TensorTerm currentGeneratedTensorTerm(originalTensorTerm);
		uint32_t mask = 1;

		int locationOfIndexToChoose = numberOfIndices - 1;
		for (uint32_t j = 0; j < numberOfIndices; j++) {
			uint32_t bar_state = (i & mask) >> j;

			currentGeneratedTensorTerm.fillBarsBasedOnBinaryMarker(allIndexNames[locationOfIndexToChoose], bar_state);
			locationOfIndexToChoose--;
			mask = mask << 1;
		}

		allTensorTerms.push_back(currentGeneratedTensorTerm);
	}
}

void ProductResolver::parseInput(std::string input) {
	std::regex fullTensorTokens("[A-Z]_\\{(/[a-z])*\\}");
	std::regex eachIndex("(/[a-z])");

	const std::sregex_iterator end;

	std::vector<std::string> allTensorTokens;
	std::vector<std::string> allIndices;

	for (std::sregex_iterator it(input.begin(), input.end(), fullTensorTokens); it != end; ++it) {
		if (!(it->empty())) {
			allTensorTokens.push_back(it->str());
		}
	}
	for (auto& tensorToken : allTensorTokens) {
		Tensor tempOriginalTensor;
		tempOriginalTensor.setLabel(tensorToken.substr(0, 1));
		for (std::sregex_iterator it(tensorToken.begin(), tensorToken.end(), eachIndex); it != end; ++it) {
			if (!(it->empty())) {
				//Each token is of form "/x", hence need to extract second character
				//-1 is simply to indicate that this index is a template, not a usable term
				tempOriginalTensor.addIndex(((it->str()).substr(1, 1)), -1);
			}
		}
		originalTensorTerm.addTensor(tempOriginalTensor);
	}
	allIndexNames = originalTensorTerm.getListOfNames();

}

void ProductResolver::printUnsimplifiedReducibleTensorTerms() {
	for (int i = 0; i < allTensorTerms.size() - 1; ++i) {
		allTensorTerms[i].printTensorTerm();
		cout << " + \n";
	}
	allTensorTerms[allTensorTerms.size() - 1].printTensorTerm();
	cout << endl;
}

void ProductResolver::printSimplifiedReducibleTensorTerms() {
	if (simplifiedTensorTerms.size() > 1) {
		for (int i = 0; i < simplifiedTensorTerms.size() - 1; ++i) {
			simplifiedTensorTerms[i].printTensorTerm();
			cout << " + \n";
		}
	}

	if (!simplifiedTensorTerms.empty()) {
		simplifiedTensorTerms.back().printTensorTerm();
	}
	cout << endl;
}



void ProductResolver::simplifyReducibleTensorTerms() {
	//Reorder indices all terms so they are uniform.
	for (auto& term : allTensorTerms) {
		term.reorderIndicesOfTerm();
	}

	//Sort the tensors all terms so they are uniform.
	for (auto& term : allTensorTerms) {
		term.sortTensorTerm();
	}


	//For every candidate term
	for (int i = 0; i < allTensorTerms.size(); ++i) {
		bool oneSuccessfulMatch = false;
		//Check all terms in simplified expression that has been generated so far
		for (int j = 0; j < simplifiedTensorTerms.size(); ++j) {

			if (areTensorTermsSameStructure(simplifiedTensorTerms[j], allTensorTerms[i])) {
				TensorTerm renamedTerm;
				//Attempt to rename the indices
				bool successfulRename = performRenameIfValidIncludingPermutations(simplifiedTensorTerms[j], allTensorTerms[i], renamedTerm);

				if (successfulRename) {
					renamedTerm.reorderIndicesOfTerm();
					//Technically, a successful rename, once ordered, should always be identical.
					if (areTensorTermsIdentical(simplifiedTensorTerms[j], renamedTerm)) {
						simplifiedTensorTerms[j].addTermCoefficientIntoThisTerm(renamedTerm);
						oneSuccessfulMatch = true;
						break;
					}
				}
			}
		}
		if (!oneSuccessfulMatch) {
			simplifiedTensorTerms.push_back(allTensorTerms[i]);
		}
	}

	erase0CoefficientReducibleTensorTerms();
}

void ProductResolver::erase0CoefficientReducibleTensorTerms() {
	for (int i = 0; i < simplifiedTensorTerms.size(); ++i) {
		if (simplifiedTensorTerms[i].getCoefficient() == 0) {
			simplifiedTensorTerms.erase(simplifiedTensorTerms.begin() + i);
			--i;
		}
	}
}

MathExpression ProductResolver::fullyReduceTensorTermAtLocation(int location) {
	MathExpression result;
	MathExpressionTerm tempTermFor1Coefficient;
	tempTermFor1Coefficient.setCoefficient(1);
	result.addTerm(tempTermFor1Coefficient);

	std::sort(allIndexNames.begin(), allIndexNames.end(), compareIndexStrengths);
	std::string latestName = allIndexNames.back();

	//Substitute each Reducible Tensor with the Irreducible Tensor Expression and expand
	for (int i = 0; i < simplifiedTensorTerms[location].getSize(); ++i) {
		MathExpression tempME;
		Tensor currentTensor = simplifiedTensorTerms[location].getTensor(i);
		tempME.setSubstitutionFromReducibleTensor(currentTensor, latestName);
		result = expandMathExpressions(result, tempME);
	}

	//Expand and simplify expression

	cout << "\nExpanding term " << (location + 1) << "..." << endl;
	result.expandAllLeviCivitas(predetermined2Match, predetermined1Match, predetermined0Match);

	cout << "\nSimplifying term " << (location + 1) << "..." << endl;
	result.simplifyExpressionByDeltas();
	result.simplifyExpressionBySymmetricAsymmetricProperty();
	result.simplifyExpressionByRenaming();

	//The original reducible TensorTerm's coefficient needs to be considered
	result.multiplyByCoefficient(simplifiedTensorTerms[location].getCoefficient());
	return result;

}

void ProductResolver::printMathExpression() {
	finalMathExpression.print();
}

void ProductResolver::printMathExpressionAsLatex() {
	finalMathExpression.printLatex();
}


void ProductResolver::fullyReduceTensorTerms() {
	for (int i = 0; i < simplifiedTensorTerms.size(); ++i) {
		MathExpression tempExpression = fullyReduceTensorTermAtLocation(i);
		finalMathExpression += tempExpression;
	}

	//Repeat renaming phase over the whole expression
	finalMathExpression.simplifyExpressionByRenaming();

	//The trivial fraction that arises from generating the Unsimplified Reducible Tensors
	finalMathExpression.multiplyByCoefficient(1, pow(2, allIndexNames.size()));

	//Debug
	//MathExpression tempExpression = fullyReduceTensorTermAtLocation(13);
}

void ProductResolver::normalizeIrreducibleTensorTerms() {
	finalMathExpression.normalizeAllIrreducibleTensors();
}
