#ifndef PRODUCT_RESOLVER_H
#define PRODUCT_RESOLVER_H

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <map>
#include <cmath>

#include "Tensor.h"
#include "TensorTerm.h"
#include "MathExpression.h"
#include "UtilityFunctions.h"

using std::cout;
using std::cin;
using std::endl;

class ProductResolver {
public:
	ProductResolver();

    void parseInput(std::string input);
    void generateAllUnsimplifiedReducibleTensorTerms();

	void simplifyReducibleTensorTerms();
	void fullyReduceTensorTerms();
	void normalizeIrreducibleTensorTerms();

    void printSimplifiedReducibleTensorTermsAsLatex();
	void printMathExpressionAsLatex();
	void printUnsimplifiedReducibleTensorTerms();
	void printSimplifiedReducibleTensorTerms();
	void printMathExpression();

	//Phase 2
	void setAllReducibleTensorTermsFromVector(const std::vector<TensorTerm>& inpTensorTerms) {
		allTensorTerms = inpTensorTerms;
	}
	std::vector<TensorTerm> getSimplifiedTensorTerms() const {
		return simplifiedTensorTerms;
	}

	static void setupPredeterminedLeviCivitaExpansions(MathExpression& pred2, MathExpression& pred1, MathExpression& pred0);

private:
	MathExpression fullyReduceTensorTermAtLocation(int location);
	void erase0CoefficientReducibleTensorTerms();


	std::vector<std::string> allIndexNames;
	TensorTerm originalTensorTerm;
	std::vector<TensorTerm> allTensorTerms;
	std::vector<TensorTerm> simplifiedTensorTerms;
	MathExpression finalMathExpression;

	//For determinant calcs for Levi
	MathExpression predetermined2Match;
	MathExpression predetermined1Match;
	MathExpression predetermined0Match;


};

#endif