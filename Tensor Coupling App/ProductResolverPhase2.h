#pragma once

#include "Tensor.h"
#include "TensorTerm.h"
#include "MathExpression.h"
#include "UtilityFunctions.h"

class ProductResolverPhase2 {
public:
	ProductResolverPhase2();
	
	void getInput();
	void generateRawTerms();

	void shiftBbtInRawTerms();
	void simplifyDeltasOfRawTerms();

	void simplifyRawTermsByRenaming();

	void reduceReducibleTensors();
	void substitutePsiSpinors();
	void reduceRightBbtChains();
	void evaluateChargeConjugates();
	void simplifyDeltasOfEvaluatedTerms();
	void simplifyLevisOfEvaluatedTerms();
	void simplifyEvaluatedTermsByRenaming();
	void normalizeIrreducibleTensors();



	MathExpression initialInput;
	MathExpression finalMathExpression;


	//For determinant calcs for Levi
	MathExpression predetermined2Match;
	MathExpression predetermined1Match;
	MathExpression predetermined0Match;

};