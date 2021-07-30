#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

#include "ProductResolver.hpp"
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

int main() {
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


	return 0;
}

