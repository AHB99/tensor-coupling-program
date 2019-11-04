#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

#include "ProductResolver.hpp"
#include "ProductResolverPhase2.h"
#include "Tensor.h"
#include "TensorTerm.h"
#include "UtilityFunctions.h"

#include "IrreducibleTensor.h"
#include "Delta.h"
#include "LeviCivita.h"
#include "MathExpressionTerm.h"
#include "MathExpression.h"
#include "Coefficient.h"

using std::cout;
using std::cin;
using std::endl;

void phase1UI();

int main() {

	//MathExpressionTerm tempMET;
	//tempMET.inputByUserPhase2();
	//MathExpression tempExp;
	//tempExp.addTerm(tempMET);
	//cout << "Input:" << endl;
	//tempExp.printPhase2();
	//cout << "\nResult: " << endl;

	//MathExpression generatedTerms = MathExpression::expandGammaAndInitialTensor(tempMET);
	//generatedTerms.printPhase2();
	//cout << "\nShifted form: " << endl;

	//generatedTerms.shiftAllRightBbtChainBtOperatorsToLeft();
	//generatedTerms.printPhase2();
	//
	//cout << "\nDelta summed form: " << endl;

	//generatedTerms.simplifyExpressionWithReducibleTensorsByDeltas();
	//generatedTerms.printPhase2();

	//generatedTerms.simplifyExpressionByRenamingBbtAndReducibleTensors();
	//cout << "\nRenaming simplified form: " << endl;
	//generatedTerms.printPhase2();


	////***
	ProductResolverPhase2 resolver;
	resolver.getInput();
	resolver.generateRawTerms();
	resolver.shiftBbtInRawTerms();
	resolver.simplifyDeltasOfRawTerms();
	resolver.simplifyRawTermsByRenaming();
	resolver.reduceReducibleTensors();
	resolver.substitutePsiSpinors();
	resolver.reduceRightBbtChains();
	resolver.evaluateChargeConjugates();
	resolver.simplifyDeltasOfEvaluatedTerms();
	resolver.simplifyLevisOfEvaluatedTerms();
	resolver.normalizeIrreducibleTensors();

	//MathExpressionTerm tempMET;
	//tempMET.inputByUserPhase2();
	//MathExpression tempExp;
	//tempExp.addTerm(tempMET);
	//cout << "Input:" << endl;
	//tempExp.printPhase2();

	//tempMET.reduceRightBbtChainByEvaluation();
	//cout << "Reduced: " << endl;
	//tempMET.printPhase2();

	//auto names = tempMET.getAllUniqueNamesOfTerm();
	//for (auto& name : names) {
	//	cout << name << " ";
	//}
	//cout << endl;

	//cout << "Next Name: " << getNextNameGivenTerm(tempMET) << endl;





	//if (tempMET.willBbtFormUsefulInteraction()) {
	//	cout << "Useful!" << endl;
	//}
	//else {
	//	cout << "Not useful!" << endl;
	//}

    return 0;
}

void phase1UI() {
	ProductResolver resolver;

	std::string input;
	cout << "Enter coupling in the format \"X_{/i/j/k}Y_{/i/j/k}\": " << endl;
	cin >> input;

	resolver.parseInput(input);

	resolver.generateAllUnsimplifiedReducibleTensorTerms();

	cout << "Simplified Reducible Tensors: " << endl;
	resolver.simplifyReducibleTensorTerms();
	resolver.printSimplifiedReducibleTensorTerms();
	cout << "Simplified Reducible Tensors As Latex: " << endl;
	resolver.printSimplifiedReducibleTensorTermsAsLatex();

	cout << "Reducing..." << endl;
	resolver.fullyReduceTensorTerms();
	cout << "Reduced Form as Latex:" << endl;
	resolver.printMathExpressionAsLatex();
	cout << endl;
	cout << "Normalizing..." << endl;
	resolver.normalizeIrreducibleTensorTerms();
	cout << endl;
	cout << "Normalized Form as Latex..." << endl;
	resolver.printMathExpressionAsLatex();
	cout << endl;

	char exit;
	cout << "Confirm quit (Enter any key): ";
	cin >> exit;
}