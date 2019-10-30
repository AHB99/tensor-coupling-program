#include "ProductResolverPhase2.h"


void ProductResolverPhase2::getInput() {
	MathExpressionTerm tempMET;
	tempMET.inputByUserPhase2();
	initialInput.addTerm(tempMET);

	//DEBUG
	cout << "Input:" << endl;
	initialInput.printPhase2();
}

void ProductResolverPhase2::generateRawTerms() {
	finalMathExpression = MathExpression::expandGammaAndInitialTensor(initialInput.getTermAtLocation(0));
	
	//DEBUG
	cout << "\nResult: " << endl;
	finalMathExpression.printPhase2();
}

void ProductResolverPhase2::shiftBbtInRawTerms() {
	finalMathExpression.shiftAllRightBbtChainBtOperatorsToLeft();

	//DEBUG
	cout << "\nShifted form: " << endl;
	finalMathExpression.printPhase2();

}

void ProductResolverPhase2::simplifyDeltasOfRawTerms() {
	finalMathExpression.simplifyExpressionWithReducibleTensorsByDeltas();

	//DEBUG
	cout << "\nDelta summed form: " << endl;
	finalMathExpression.printPhase2();

}

void ProductResolverPhase2::simplifyRawTermsByRenaming() {
	finalMathExpression.simplifyExpressionByRenamingBbtAndReducibleTensors();

	//DEBUG
	cout << "\nRenaming simplified form: " << endl;
	finalMathExpression.printPhase2();
}

void ProductResolverPhase2::reduceReducibleTensors() {
	MathExpression newFinalMathExpression;
	for (int i = 0; i < finalMathExpression.getSize(); ++i) {
		MathExpression currentTermExpression;
		MathExpressionTerm currentTerm(finalMathExpression.getTermAtLocation(i));
		currentTerm.clearReducibleTensors();
		currentTermExpression.addTerm(currentTerm);

		std::string latestName = initialInput.getTermAtLocation(0).getInitialIndicesFromGamma().back();

		for (int j = 0; j < finalMathExpression.getTermAtLocation(i).getNumberOfReducibleTensors(); ++j) {
			MathExpression tempME;
			Tensor currentTensor = finalMathExpression.getTermAtLocation(i).getReducibleTensorAt(j);
			tempME.setSubstitutionFromReducibleTensor(currentTensor, latestName);
			currentTermExpression = expandMathExpressions(currentTermExpression, tempME);
		}

		newFinalMathExpression += currentTermExpression;
	}
	finalMathExpression = newFinalMathExpression;

	//DEBUG
	cout << "\nReduced form: " << endl;
	finalMathExpression.printPhase2();
}

void ProductResolverPhase2::substitutePsiSpinors() {
	MathExpression newFinalMathExpression;
	for (int i = 0; i < finalMathExpression.getSize(); ++i) {
		std::string currentLatestName{ getNextNameGivenTerm(finalMathExpression.getTermAtLocation(i)) };
		MathExpression substitutedExpression = finalMathExpression.getTermAtLocation(i).performPsiSubstitution(currentLatestName);
	
		//Time saver
		for (int j = 0; j < substitutedExpression.getSize(); ++j) {
			if (!substitutedExpression.getTermAtLocation(j).willBbtFormUsefulInteraction()) {
				substitutedExpression.setCoefficientOfTermAtLocation(j, Coefficient(0));
			}
		}

		newFinalMathExpression += substitutedExpression;
	}
	finalMathExpression = newFinalMathExpression;
	finalMathExpression.erase0Terms();
	//DEBUG
	cout << "\nPsi Substituted: " << endl;
	finalMathExpression.printPhase2();
}

void ProductResolverPhase2::reduceRightBbtChains() {
	finalMathExpression.reduceRightBbtChainsByEvaluation();
	//DEBUG
	cout << "\nBbt reduced: " << endl;
	finalMathExpression.printPhase2();
}