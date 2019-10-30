#pragma once

#include "Tensor.h"
#include "TensorTerm.h"
#include "MathExpression.h"
#include "UtilityFunctions.h"

class ProductResolverPhase2 {
public:
	
	void getInput();
	void generateRawTerms();

	void shiftBbtInRawTerms();
	void simplifyDeltasOfRawTerms();

	void simplifyRawTermsByRenaming();

	void reduceReducibleTensors();
	void substitutePsiSpinors();
	void reduceRightBbtChains();

	MathExpression initialInput;
	MathExpression finalMathExpression;
};