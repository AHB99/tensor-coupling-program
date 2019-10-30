#include "MathExpression.h"
#include "Tensor.h"
#include "UtilityFunctions.h"
#include "ProductResolver.hpp"

void MathExpression::print() const {
	if (mathExpressionTerms.empty()) {
		cout << 0 << endl;
	}
	for (auto& MET : mathExpressionTerms) {
		if (MET.getCoefficient() > 0) {
			cout << "+";
		}
		MET.print();
		cout << " ";
	}
}

void MathExpression::printLatex() const {
	if (mathExpressionTerms.empty()) {
		cout << 0 << endl;
	}
	for (auto& MET : mathExpressionTerms) {
		MET.printLatex();
		cout << " ";
	}
}


void MathExpression::inputByUser() {
	int tempNum;
	cout << "How many terms: ";
	cin >> tempNum;
	for (int i = 0; i < tempNum; ++i) {
		MathExpressionTerm tempMET;
		tempMET.inputByUser();
		mathExpressionTerms.push_back(tempMET);
	}
}

MathExpressionTerm MathExpression::getTermAtLocation(int location) const {
	return mathExpressionTerms[location];
}

void MathExpression::addTerm(const MathExpressionTerm& met) {
	mathExpressionTerms.push_back(met);
}

void MathExpression::simplifyCoefficientsOfAllTerms() {
	for (auto& MET : mathExpressionTerms) {
		MET.simplifyCoefficient();
	}
}


void MathExpression::simplifyExpressionByDeltas() {

	//int i = 0;
	//while (i < mathExpressionTerms.size()) {
	//	//If the term got cancelled, delete it, else increment the loop.
	//	if (!mathExpressionTerms[i].simplifyTermByDeltas()) {
	//		mathExpressionTerms.erase(mathExpressionTerms.begin() + i);
	//	}
	//	else {
	//		++i;
	//	}
	//}

	//Possible efficient redo.

	for (auto& MET : mathExpressionTerms) {
		MET.simplifyTermByDeltas();
	}
	mathExpressionTerms.erase(std::remove_if(mathExpressionTerms.begin(), mathExpressionTerms.end(), isCoefficientZero), mathExpressionTerms.end());
}



void MathExpression::sumOverAllDeltas() {
	for (auto& MET : mathExpressionTerms) {
		MET.sumOverDeltas();
	}
}



void MathExpression::setSubstitutionFor2Index0Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 2);
	for (int i = 0; i < 2; ++i) {
		tempIrrTensor.addUpperIndex(sourceReducibleTensor.getIndexNameAt(i));
	}
	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.setCoefficient(1);

	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor2Index2Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false,2);
	for (int i = 0; i < 2; ++i) {
		tempIrrTensor.addLowerIndex(sourceReducibleTensor.getIndexNameAt(i));
	}
	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.setCoefficient(1);
	
	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor2Index1Bar(Tensor& sourceReducibleTensor) {
	std::vector<MathExpressionTerm> tempMETs(2);
	std::vector<IrreducibleTensor> tempIrrTensors;
	for (int i = 0; i < 2; ++i) {
		IrreducibleTensor tempIrrTensor(false, 2);
		tempIrrTensors.push_back(tempIrrTensor);
	}

	//First Term
	tempIrrTensors[0].addUpperIndex(sourceReducibleTensor.getIndexNameAt(1));
	tempIrrTensors[0].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));
	tempMETs[0].addIrreducibleTensor(tempIrrTensors[0]);
	tempMETs[0].setCoefficient(-1);

	//Second Term
	Delta tempDelta(sourceReducibleTensor.getIndexNameAt(1), sourceReducibleTensor.getIndexNameAt(0));
	tempMETs[1].addDelta(tempDelta);
	tempMETs[1].addIrreducibleTensor(tempIrrTensors[1]);
	tempMETs[1].setCoefficient(-1,5);

	//Add terms to Expression
	for (auto& tempMET : tempMETs) {
		addTerm(tempMET);
	}
}

void MathExpression::setSubstitutionFor2Index1Duplicate1Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 2);
	tempMET.addIrreducibleTensor(tempIrrTensor);
	//-1 because bars to left convention
	tempMET.setCoefficient(-1);
	addTerm(tempMET);
}


void MathExpression::setSubstitutionFor2Index(Tensor& sourceReducibleTensor) {
	int numOfBarredIndices = sourceReducibleTensor.getNumberOfBarredIndices();
	int numOfDuplicatedIndices = sourceReducibleTensor.getNumberOfDuplicatedIndices();

	if (numOfDuplicatedIndices == 1) {
		if (numOfBarredIndices == 1) {
			setSubstitutionFor2Index1Duplicate1Bar(sourceReducibleTensor);
		}
	}
	else if (numOfDuplicatedIndices == 0) {
		if (numOfBarredIndices == 0) {
			setSubstitutionFor2Index0Bar(sourceReducibleTensor);
		}
		else if (numOfBarredIndices == 1) {
			setSubstitutionFor2Index1Bar(sourceReducibleTensor);
		}
		else if (numOfBarredIndices == 2) {
			setSubstitutionFor2Index2Bar(sourceReducibleTensor);
		}
	}
	
}

void MathExpression::setSubstitutionFor3Index0Bar(Tensor& sourceReducibleTensor, std::string& latestName) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 3);
	LeviCivita tempLevi(true);
	for (int i = 0; i < 3; ++i) {
		tempLevi.addIndex(sourceReducibleTensor.getIndexNameAt(i));
	}
	for (int i = 0; i < 2; ++i) {
		latestName = getNextName(latestName);
		tempLevi.addIndex(latestName);
		tempIrrTensor.addLowerIndex(latestName);
	}
	
	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.addLeviCivita(tempLevi);
	tempMET.setCoefficient(1);

	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor3Index1Bar(Tensor& sourceReducibleTensor, std::string& latestName) {
	std::vector<MathExpressionTerm> tempMETs(3);
	std::vector<IrreducibleTensor> tempIrrTensors;

	for (int i = 0; i < 3; ++i) {
		IrreducibleTensor tempIrrTensor(false, 3);
		tempIrrTensors.push_back(tempIrrTensor);
	}

	//First Term
	for (int i = 0; i < 2; ++i) {
		tempIrrTensors[0].addUpperIndex(sourceReducibleTensor.getIndexNameAt(i+1));
	}
	tempIrrTensors[0].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));

	tempMETs[0].addIrreducibleTensor(tempIrrTensors[0]);
	tempMETs[0].setCoefficient(1);

	//Second Term
	tempIrrTensors[1].addUpperIndex(sourceReducibleTensor.getIndexNameAt(2));
	Delta delta1(sourceReducibleTensor.getIndexNameAt(1), sourceReducibleTensor.getIndexNameAt(0));
	tempMETs[1].addIrreducibleTensor(tempIrrTensors[1]);
	tempMETs[1].addDelta(delta1);
	tempMETs[1].setCoefficient(1,4);

	//Third term
	tempIrrTensors[2].addUpperIndex(sourceReducibleTensor.getIndexNameAt(1));
	Delta delta2(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(0));
	tempMETs[2].addIrreducibleTensor(tempIrrTensors[2]);
	tempMETs[2].addDelta(delta2);
	tempMETs[2].setCoefficient(-1,4);

	//Add terms to Expression
	for (auto& tempMET : tempMETs) {
		addTerm(tempMET);
	}
}

void MathExpression::setSubstitutionFor3Index2Bar(Tensor& sourceReducibleTensor, std::string& latestName) {
	std::vector<MathExpressionTerm> tempMETs(3);
	std::vector<IrreducibleTensor> tempIrrTensors;

	for (int i = 0; i < 3; ++i) {
		IrreducibleTensor tempIrrTensor(false, 3);
		tempIrrTensors.push_back(tempIrrTensor);
	}

	//First Term
	tempIrrTensors[0].addUpperIndex(sourceReducibleTensor.getIndexNameAt(2));
	for (int i = 0; i < 2; ++i) {
		tempIrrTensors[0].addLowerIndex(sourceReducibleTensor.getIndexNameAt(i));
	}
	tempMETs[0].addIrreducibleTensor(tempIrrTensors[0]);
	tempMETs[0].setCoefficient(1);

	//Second Term
	tempIrrTensors[1].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));
	Delta delta1(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(0));
	tempMETs[1].addIrreducibleTensor(tempIrrTensors[1]);
	tempMETs[1].addDelta(delta1);
	tempMETs[1].setCoefficient(1,4);

	//Third term
	tempIrrTensors[2].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));
	Delta delta2(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(1));
	tempMETs[2].addIrreducibleTensor(tempIrrTensors[2]);
	tempMETs[2].addDelta(delta2);
	tempMETs[2].setCoefficient(-1,4);

	//Add terms to Expression
	for (auto& tempMET : tempMETs) {
		addTerm(tempMET);
	}
}

void MathExpression::setSubstitutionFor3Index3Bar(Tensor& sourceReducibleTensor, std::string& latestName) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 3);
	LeviCivita tempLevi(false);
	for (int i = 0; i < 3; ++i) {
		tempLevi.addIndex(sourceReducibleTensor.getIndexNameAt(i));
	}
	for (int i = 0; i < 2; ++i) {
		latestName = getNextName(latestName);
		tempLevi.addIndex(latestName);
		tempIrrTensor.addUpperIndex(latestName);
	}

	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.addLeviCivita(tempLevi);
	tempMET.setCoefficient(1);

	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor3Index1Duplicate1Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 3);

	TensorTerm shiftedTensorTerm = getTensorTermWithDuplicatedIndicesToLeft(sourceReducibleTensor);
	Tensor shiftedTensor(shiftedTensorTerm.getTensor(0));
	

	tempIrrTensor.addUpperIndex(shiftedTensor.getIndexNameAt(2));

	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.setCoefficient(shiftedTensorTerm.getCoefficient());
	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor3Index1Duplicate2Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 3);

	TensorTerm shiftedTensorTerm = getTensorTermWithDuplicatedIndicesToLeft(sourceReducibleTensor);
	Tensor shiftedTensor(shiftedTensorTerm.getTensor(0));


	tempIrrTensor.addLowerIndex(shiftedTensor.getIndexNameAt(2));

	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.setCoefficient(shiftedTensorTerm.getCoefficient());
	addTerm(tempMET);
}


void MathExpression::setSubstitutionFor3Index(Tensor& sourceReducibleTensor, std::string& latestName) {
	int numOfBarredIndices = sourceReducibleTensor.getNumberOfBarredIndices();
	int numOfDuplicatedIndices = sourceReducibleTensor.getNumberOfDuplicatedIndices();

	if (numOfDuplicatedIndices == 1) {
		if (numOfBarredIndices == 1) {
			setSubstitutionFor3Index1Duplicate1Bar(sourceReducibleTensor);
		}
		else if (numOfBarredIndices == 2) {
			setSubstitutionFor3Index1Duplicate2Bar(sourceReducibleTensor);
		}
	}
	else if (numOfDuplicatedIndices == 0) {
		if (numOfBarredIndices == 0) {
			setSubstitutionFor3Index0Bar(sourceReducibleTensor, latestName);
		}
		else if (numOfBarredIndices == 1) {
			setSubstitutionFor3Index1Bar(sourceReducibleTensor, latestName);
		}
		else if (numOfBarredIndices == 2) {
			setSubstitutionFor3Index2Bar(sourceReducibleTensor, latestName);
		}
		else if (numOfBarredIndices == 3) {
			setSubstitutionFor3Index3Bar(sourceReducibleTensor, latestName);
		}
	}
}

void MathExpression::setSubstitutionFor1Index0Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 1);
	tempIrrTensor.addUpperIndex(sourceReducibleTensor.getIndexNameAt(0));
	
	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.setCoefficient(1);

	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor1Index1Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 1);
	tempIrrTensor.addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));

	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.setCoefficient(1);

	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor1Index(Tensor& sourceReducibleTensor) {
	int numOfBarredIndices = sourceReducibleTensor.getNumberOfBarredIndices();
	if (numOfBarredIndices == 0) {
		setSubstitutionFor1Index0Bar(sourceReducibleTensor);
	}
	else if (numOfBarredIndices == 1) {
		setSubstitutionFor1Index1Bar(sourceReducibleTensor);
	}
}

void MathExpression::setSubstitutionFor4Index0Bar(Tensor& sourceReducibleTensor, std::string& latestName) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 4);
	LeviCivita tempLevi(true);
	for (int i = 0; i < 4; ++i) {
		tempLevi.addIndex(sourceReducibleTensor.getIndexNameAt(i));
	}
	latestName = getNextName(latestName);
	tempLevi.addIndex(latestName);
	tempIrrTensor.addLowerIndex(latestName);
	

	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.addLeviCivita(tempLevi);
	tempMET.setCoefficient(1,24);

	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor4Index1Bar(Tensor& sourceReducibleTensor, std::string& latestName) {
	std::vector<MathExpressionTerm> tempMETs(4);
	std::vector<IrreducibleTensor> tempIrrTensors;

	for (int i = 0; i < 4; ++i) {
		IrreducibleTensor tempIrrTensor(false, 4);
		tempIrrTensors.push_back(tempIrrTensor);
	}

	//First Term
	for (int i = 0; i < 3; ++i) {
		tempIrrTensors[0].addUpperIndex(sourceReducibleTensor.getIndexNameAt(i + 1));
	}
	tempIrrTensors[0].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));

	tempMETs[0].addIrreducibleTensor(tempIrrTensors[0]);
	tempMETs[0].setCoefficient(-1);

	//Second Term
	tempIrrTensors[1].addUpperIndex(sourceReducibleTensor.getIndexNameAt(1));
	tempIrrTensors[1].addUpperIndex(sourceReducibleTensor.getIndexNameAt(2));

	Delta delta1(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(0));
	tempMETs[1].addIrreducibleTensor(tempIrrTensors[1]);
	tempMETs[1].addDelta(delta1);
	tempMETs[1].setCoefficient(-1,3);

	//Third term
	tempIrrTensors[2].addUpperIndex(sourceReducibleTensor.getIndexNameAt(1));
	tempIrrTensors[2].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));

	Delta delta2(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(0));
	tempMETs[2].addIrreducibleTensor(tempIrrTensors[2]);
	tempMETs[2].addDelta(delta2);
	tempMETs[2].setCoefficient(1,3);

	//Fourth term
	tempIrrTensors[3].addUpperIndex(sourceReducibleTensor.getIndexNameAt(2));
	tempIrrTensors[3].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));
	
	Delta delta3(sourceReducibleTensor.getIndexNameAt(1), sourceReducibleTensor.getIndexNameAt(0));
	tempMETs[3].addIrreducibleTensor(tempIrrTensors[3]);
	tempMETs[3].addDelta(delta3);
	tempMETs[3].setCoefficient(-1,3);

	//Add terms to Expression
	for (auto& tempMET : tempMETs) {
		addTerm(tempMET);
	}
}

void MathExpression::setSubstitutionFor4Index2Bar(Tensor& sourceReducibleTensor, std::string& latestName) {
	std::vector<MathExpressionTerm> tempMETs(7);
	std::vector<IrreducibleTensor> tempIrrTensors;

	for (int i = 0; i < 7; ++i) {
		IrreducibleTensor tempIrrTensor(false, 4);
		tempIrrTensors.push_back(tempIrrTensor);
	}

	//First Term
	for (int i = 0; i < 2; ++i) {
		tempIrrTensors[0].addUpperIndex(sourceReducibleTensor.getIndexNameAt(i + 2));
		tempIrrTensors[0].addLowerIndex(sourceReducibleTensor.getIndexNameAt(i));
	}

	tempMETs[0].addIrreducibleTensor(tempIrrTensors[0]);
	tempMETs[0].setCoefficient(1);

	//Second Term
	tempIrrTensors[1].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));
	tempIrrTensors[1].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));

	Delta delta1(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(1));
	tempMETs[1].addIrreducibleTensor(tempIrrTensors[1]);
	tempMETs[1].addDelta(delta1);
	tempMETs[1].setCoefficient(1,3);

	//Third term
	tempIrrTensors[2].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));
	tempIrrTensors[2].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));

	Delta delta2(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(0));
	tempMETs[2].addIrreducibleTensor(tempIrrTensors[2]);
	tempMETs[2].addDelta(delta2);
	tempMETs[2].setCoefficient(-1,3);

	//Fourth term
	tempIrrTensors[3].addUpperIndex(sourceReducibleTensor.getIndexNameAt(2));
	tempIrrTensors[3].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));

	Delta delta3(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(0));
	tempMETs[3].addIrreducibleTensor(tempIrrTensors[3]);
	tempMETs[3].addDelta(delta3);
	tempMETs[3].setCoefficient(1,3);

	//Fifth term
	tempIrrTensors[4].addUpperIndex(sourceReducibleTensor.getIndexNameAt(2));
	tempIrrTensors[4].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));

	Delta delta4(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(1));
	tempMETs[4].addIrreducibleTensor(tempIrrTensors[4]);
	tempMETs[4].addDelta(delta4);
	tempMETs[4].setCoefficient(-1,3);

	//Sixth term
	Delta delta5(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(1));
	Delta delta6(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(0));

	tempMETs[5].addIrreducibleTensor(tempIrrTensors[5]);

	tempMETs[5].addDelta(delta5);
	tempMETs[5].addDelta(delta6);

	tempMETs[5].setCoefficient(1,20);

	//Seventh term
	Delta delta7(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(0));
	Delta delta8(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[6].addIrreducibleTensor(tempIrrTensors[6]);

	tempMETs[6].addDelta(delta7);
	tempMETs[6].addDelta(delta8);

	tempMETs[6].setCoefficient(-1,20);

	//Add terms to Expression
	for (auto& tempMET : tempMETs) {
		addTerm(tempMET);
	}
}

void MathExpression::setSubstitutionFor4Index3Bar(Tensor& sourceReducibleTensor, std::string& latestName) {
	//REFERENCCE already has this sorted correctly, no negative.
	std::vector<MathExpressionTerm> tempMETs(4);
	std::vector<IrreducibleTensor> tempIrrTensors;

	for (int i = 0; i < 4; ++i) {
		IrreducibleTensor tempIrrTensor(false, 4);
		tempIrrTensors.push_back(tempIrrTensor);
	}

	//First Term
	for (int i = 0; i < 3; ++i) {
		tempIrrTensors[0].addLowerIndex(sourceReducibleTensor.getIndexNameAt(i));
	}
	tempIrrTensors[0].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));

	tempMETs[0].addIrreducibleTensor(tempIrrTensors[0]);
	tempMETs[0].setCoefficient(1);

	//Second Term
	tempIrrTensors[1].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));
	tempIrrTensors[1].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));

	Delta delta1(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(2));
	tempMETs[1].addIrreducibleTensor(tempIrrTensors[1]);
	tempMETs[1].addDelta(delta1);
	tempMETs[1].setCoefficient(1,3);

	//Third term
	tempIrrTensors[2].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));
	tempIrrTensors[2].addLowerIndex(sourceReducibleTensor.getIndexNameAt(2));

	Delta delta2(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(1));
	tempMETs[2].addIrreducibleTensor(tempIrrTensors[2]);
	tempMETs[2].addDelta(delta2);
	tempMETs[2].setCoefficient(-1,3);

	//Fourth term
	tempIrrTensors[3].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));
	tempIrrTensors[3].addLowerIndex(sourceReducibleTensor.getIndexNameAt(2));

	Delta delta3(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(0));
	tempMETs[3].addIrreducibleTensor(tempIrrTensors[3]);
	tempMETs[3].addDelta(delta3);
	tempMETs[3].setCoefficient(1,3);

	//Add terms to Expression
	for (auto& tempMET : tempMETs) {
		addTerm(tempMET);
	}
}

void MathExpression::setSubstitutionFor4Index4Bar(Tensor& sourceReducibleTensor, std::string& latestName) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 4);
	LeviCivita tempLevi(false);
	for (int i = 0; i < 4; ++i) {
		tempLevi.addIndex(sourceReducibleTensor.getIndexNameAt(i));
	}
	latestName = getNextName(latestName);
	tempLevi.addIndex(latestName);
	tempIrrTensor.addUpperIndex(latestName);


	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.addLeviCivita(tempLevi);
	tempMET.setCoefficient(1,24);

	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor4Index1Duplicate1Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 4);

	TensorTerm shiftedTensorTerm = getTensorTermWithDuplicatedIndicesToLeft(sourceReducibleTensor);
	Tensor shiftedTensor(shiftedTensorTerm.getTensor(0));

	tempIrrTensor.addUpperIndex(shiftedTensor.getIndexNameAt(2));
	tempIrrTensor.addUpperIndex(shiftedTensor.getIndexNameAt(3));


	tempMET.addIrreducibleTensor(tempIrrTensor);
	//-1 because REFERENCE would be negative if shifted to form
	tempMET.setCoefficient(shiftedTensorTerm.getCoefficient() * -1);
	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor4Index1Duplicate2Bar(Tensor& sourceReducibleTensor) {
	std::vector<MathExpressionTerm> tempMETs(2);
	std::vector<IrreducibleTensor> tempIrreducibleTensors;

	for (int i = 0; i < 2; ++i) {
		IrreducibleTensor t(false, 4);
		tempIrreducibleTensors.push_back(t);
	}

	TensorTerm shiftedTensorTerm = getTensorTermWithDuplicatedIndicesToLeft(sourceReducibleTensor);
	Tensor shiftedTensor(shiftedTensorTerm.getTensor(0));

	//Term 1
	//Note, REFERENCE substitution has bars on left, so we adjust for that here
	tempIrreducibleTensors[0].addUpperIndex(shiftedTensor.getIndexNameAt(3));
	tempIrreducibleTensors[0].addLowerIndex(shiftedTensor.getIndexNameAt(2));


	tempMETs[0].addIrreducibleTensor(tempIrreducibleTensors[0]);

	//Term 2
	Delta tempDelta;
	tempDelta.setIndices(shiftedTensor.getIndexNameAt(3), shiftedTensor.getIndexNameAt(2));
	tempMETs[1].addDelta(tempDelta);
	tempMETs[1].addIrreducibleTensor(tempIrreducibleTensors[1]);
	tempMETs[1].setCoefficient(1, 5);

	//Add terms
	for (auto& met : tempMETs) {
		addTerm(met);
	}

	multiplyByCoefficient(shiftedTensorTerm.getCoefficient());
}

void MathExpression::setSubstitutionFor4Index1Duplicate3Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 4);

	TensorTerm shiftedTensorTerm = getTensorTermWithDuplicatedIndicesToLeft(sourceReducibleTensor);
	Tensor shiftedTensor(shiftedTensorTerm.getTensor(0));

	tempIrrTensor.addLowerIndex(shiftedTensor.getIndexNameAt(2));
	tempIrrTensor.addLowerIndex(shiftedTensor.getIndexNameAt(3));


	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.setCoefficient(shiftedTensorTerm.getCoefficient());
	addTerm(tempMET);
}


void MathExpression::setSubstitutionFor4Index2Duplicate2Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 4);

	TensorTerm shiftedTensorTerm = getTensorTermWithDuplicatedIndicesToLeft(sourceReducibleTensor);
	Tensor shiftedTensor(shiftedTensorTerm.getTensor(0));

	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.setCoefficient(shiftedTensorTerm.getCoefficient());
	addTerm(tempMET);
}


void MathExpression::setSubstitutionFor4Index(Tensor& sourceReducibleTensor, std::string& latestName) {
	int numOfBarredIndices = sourceReducibleTensor.getNumberOfBarredIndices();
	int numOfDuplicatedIndices = sourceReducibleTensor.getNumberOfDuplicatedIndices();


	if (numOfDuplicatedIndices == 2) {
		if (numOfBarredIndices == 2) {
			setSubstitutionFor4Index2Duplicate2Bar(sourceReducibleTensor);
		}
	}
	else if (numOfDuplicatedIndices == 1) {
		if (numOfBarredIndices == 1) {
			setSubstitutionFor4Index1Duplicate1Bar(sourceReducibleTensor);
		}
		else if (numOfBarredIndices == 2) {
			setSubstitutionFor4Index1Duplicate2Bar(sourceReducibleTensor);
		}
		else if (numOfBarredIndices == 3) {
			setSubstitutionFor4Index1Duplicate3Bar(sourceReducibleTensor);
		}
	}
	else if (numOfDuplicatedIndices == 0) {
		if (numOfBarredIndices == 0) {
			setSubstitutionFor4Index0Bar(sourceReducibleTensor, latestName);
		}
		else if (numOfBarredIndices == 1) {
			setSubstitutionFor4Index1Bar(sourceReducibleTensor, latestName);
		}
		else if (numOfBarredIndices == 2) {
			setSubstitutionFor4Index2Bar(sourceReducibleTensor, latestName);
		}
		else if (numOfBarredIndices == 3) {
			setSubstitutionFor4Index3Bar(sourceReducibleTensor, latestName);
		}
		else if (numOfBarredIndices == 4) {
			setSubstitutionFor4Index4Bar(sourceReducibleTensor, latestName);
		}
	}
	
		
}

void MathExpression::setSubstitutionFor5Index0Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 5);
	LeviCivita tempLevi(true);
	for (int i = 0; i < 5; ++i) {
		tempLevi.addIndex(sourceReducibleTensor.getIndexNameAt(i));
	}

	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.addLeviCivita(tempLevi);
	tempMET.setCoefficient(1);

	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor5Index1Bar(Tensor& sourceReducibleTensor, std::string& latestName) {
	std::vector<MathExpressionTerm> tempMETs(5);
	std::vector<IrreducibleTensor> tempIrrTensors;
	std::vector<LeviCivita> tempLevis;
	IrreducibleTensor tempSymIrrTensor(true, 5);
	tempIrrTensors.push_back(tempSymIrrTensor);

	for (int i = 0; i < 4; ++i) {
		IrreducibleTensor tempIrrTensor(false, 5);
		tempIrrTensors.push_back(tempIrrTensor);
	}
	for (int i = 0; i < 5; ++i) {
		LeviCivita tempLevi(true);
		tempLevis.push_back(tempLevi);
	}

	//First Term
	for (int i = 0; i < 4; ++i) {
		tempLevis[0].addIndex(sourceReducibleTensor.getIndexNameAt(i+1));
	}
	latestName = getNextName(latestName);
	tempLevis[0].addIndex(latestName);
	tempIrrTensors[0].addLowerIndex(latestName);
	tempIrrTensors[0].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));

	tempMETs[0].addIrreducibleTensor(tempIrrTensors[0]);
	tempMETs[0].addLeviCivita(tempLevis[0]);
	tempMETs[0].setCoefficient(1);
	std::string copyOfFirstNameObtained(latestName);

	//Second Term
	latestName = getNextName(latestName);
	tempIrrTensors[1].addLowerIndex(copyOfFirstNameObtained);
	tempIrrTensors[1].addLowerIndex(latestName);

	Delta delta1(sourceReducibleTensor.getIndexNameAt(1), sourceReducibleTensor.getIndexNameAt(0));
	for (int i = 0; i < 3; ++i) {
		tempLevis[1].addIndex(sourceReducibleTensor.getIndexNameAt(i+2));
	}
	tempLevis[1].addIndex(copyOfFirstNameObtained);
	tempLevis[1].addIndex(latestName);

	tempMETs[1].addIrreducibleTensor(tempIrrTensors[1]);
	tempMETs[1].addDelta(delta1);
	tempMETs[1].addLeviCivita(tempLevis[1]);

	tempMETs[1].setCoefficient(1,2);

	//Third Term
	tempIrrTensors[2].addLowerIndex(copyOfFirstNameObtained);
	tempIrrTensors[2].addLowerIndex(latestName);

	Delta delta2(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(0));
	tempLevis[2].addIndex(sourceReducibleTensor.getIndexNameAt(1));
	tempLevis[2].addIndex(sourceReducibleTensor.getIndexNameAt(3));
	tempLevis[2].addIndex(sourceReducibleTensor.getIndexNameAt(4));


	tempLevis[2].addIndex(copyOfFirstNameObtained);
	tempLevis[2].addIndex(latestName);

	tempMETs[2].addIrreducibleTensor(tempIrrTensors[2]);
	tempMETs[2].addDelta(delta2);
	tempMETs[2].addLeviCivita(tempLevis[2]);

	tempMETs[2].setCoefficient(-1,2);

	//Fourth Term
	tempIrrTensors[3].addLowerIndex(copyOfFirstNameObtained);
	tempIrrTensors[3].addLowerIndex(latestName);

	Delta delta3(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(0));
	tempLevis[3].addIndex(sourceReducibleTensor.getIndexNameAt(1));
	tempLevis[3].addIndex(sourceReducibleTensor.getIndexNameAt(2));
	tempLevis[3].addIndex(sourceReducibleTensor.getIndexNameAt(4));


	tempLevis[3].addIndex(copyOfFirstNameObtained);
	tempLevis[3].addIndex(latestName);

	tempMETs[3].addIrreducibleTensor(tempIrrTensors[3]);
	tempMETs[3].addDelta(delta3);
	tempMETs[3].addLeviCivita(tempLevis[3]);

	tempMETs[3].setCoefficient(1,2);

	//Fifth Term
	tempIrrTensors[4].addLowerIndex(copyOfFirstNameObtained);
	tempIrrTensors[4].addLowerIndex(latestName);

	Delta delta4(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(0));
	tempLevis[4].addIndex(sourceReducibleTensor.getIndexNameAt(1));
	tempLevis[4].addIndex(sourceReducibleTensor.getIndexNameAt(2));
	tempLevis[4].addIndex(sourceReducibleTensor.getIndexNameAt(3));


	tempLevis[4].addIndex(copyOfFirstNameObtained);
	tempLevis[4].addIndex(latestName);

	tempMETs[4].addIrreducibleTensor(tempIrrTensors[4]);
	tempMETs[4].addDelta(delta4);
	tempMETs[4].addLeviCivita(tempLevis[4]);

	tempMETs[4].setCoefficient(-1,2);

	//Add terms to Expression
	for (auto& tempMET : tempMETs) {
		addTerm(tempMET);
	}
}

void MathExpression::setSubstitutionFor5Index2Bar(Tensor& sourceReducibleTensor) {
	std::vector<MathExpressionTerm> tempMETs(13);
	std::vector<IrreducibleTensor> tempIrrTensors;

	for (int i = 0; i < 13; ++i) {
		IrreducibleTensor tempIrrTensor(false, 5);
		tempIrrTensors.push_back(tempIrrTensor);
	}

	//First Term
	for (int i = 0; i < 3; ++i) {
		tempIrrTensors[0].addUpperIndex(sourceReducibleTensor.getIndexNameAt(i + 2));
	}
	for (int i = 0; i < 2; ++i) {
		tempIrrTensors[0].addLowerIndex(sourceReducibleTensor.getIndexNameAt(i));
	}


	tempMETs[0].addIrreducibleTensor(tempIrrTensors[0]);
	tempMETs[0].setCoefficient(1);

	//Second Term
	tempIrrTensors[1].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));
	tempIrrTensors[1].addUpperIndex(sourceReducibleTensor.getIndexNameAt(4));

	tempIrrTensors[1].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));

	Delta delta1(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(0));

	tempMETs[1].addIrreducibleTensor(tempIrrTensors[1]);
	tempMETs[1].addDelta(delta1);
	tempMETs[1].setCoefficient(1,2);

	//Third Term
	tempIrrTensors[2].addUpperIndex(sourceReducibleTensor.getIndexNameAt(2));
	tempIrrTensors[2].addUpperIndex(sourceReducibleTensor.getIndexNameAt(4));

	tempIrrTensors[2].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));

	Delta delta2(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(0));

	tempMETs[2].addIrreducibleTensor(tempIrrTensors[2]);
	tempMETs[2].addDelta(delta2);
	tempMETs[2].setCoefficient(-1,2);

	//Fourth Term
	tempIrrTensors[3].addUpperIndex(sourceReducibleTensor.getIndexNameAt(2));
	tempIrrTensors[3].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));

	tempIrrTensors[3].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));

	Delta delta3(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(0));

	tempMETs[3].addIrreducibleTensor(tempIrrTensors[3]);
	tempMETs[3].addDelta(delta3);
	tempMETs[3].setCoefficient(1,2);

	//Fifth Term
	tempIrrTensors[4].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));
	tempIrrTensors[4].addUpperIndex(sourceReducibleTensor.getIndexNameAt(4));

	tempIrrTensors[4].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));

	Delta delta4(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[4].addIrreducibleTensor(tempIrrTensors[4]);
	tempMETs[4].addDelta(delta4);
	tempMETs[4].setCoefficient(-1,2);

	//Sixth Term
	tempIrrTensors[5].addUpperIndex(sourceReducibleTensor.getIndexNameAt(2));
	tempIrrTensors[5].addUpperIndex(sourceReducibleTensor.getIndexNameAt(4));

	tempIrrTensors[5].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));

	Delta delta5(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[5].addIrreducibleTensor(tempIrrTensors[5]);
	tempMETs[5].addDelta(delta5);
	tempMETs[5].setCoefficient(1,2);

	//Seventh Term
	tempIrrTensors[6].addUpperIndex(sourceReducibleTensor.getIndexNameAt(2));
	tempIrrTensors[6].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));

	tempIrrTensors[6].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));

	Delta delta6(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[6].addIrreducibleTensor(tempIrrTensors[6]);
	tempMETs[6].addDelta(delta6);
	tempMETs[6].setCoefficient(-1,2);

	//Eighth Term
	tempIrrTensors[7].addUpperIndex(sourceReducibleTensor.getIndexNameAt(4));

	Delta delta7(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(0));
	Delta delta8(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[7].addIrreducibleTensor(tempIrrTensors[7]);
	tempMETs[7].addDelta(delta7);
	tempMETs[7].addDelta(delta8);

	tempMETs[7].setCoefficient(1,12);

	//Ninth Term
	tempIrrTensors[8].addUpperIndex(sourceReducibleTensor.getIndexNameAt(4));

	Delta delta9(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(0));
	Delta delta10(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[8].addIrreducibleTensor(tempIrrTensors[8]);
	tempMETs[8].addDelta(delta9);
	tempMETs[8].addDelta(delta10);

	tempMETs[8].setCoefficient(-1,12);

	//Tenth Term
	tempIrrTensors[9].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));

	Delta delta11(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(0));
	Delta delta12(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[9].addIrreducibleTensor(tempIrrTensors[9]);
	tempMETs[9].addDelta(delta11);
	tempMETs[9].addDelta(delta12);

	tempMETs[9].setCoefficient(-1,12);

	//Eleventh Term
	tempIrrTensors[10].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));

	Delta delta13(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(0));
	Delta delta14(sourceReducibleTensor.getIndexNameAt(2), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[10].addIrreducibleTensor(tempIrrTensors[10]);
	tempMETs[10].addDelta(delta13);
	tempMETs[10].addDelta(delta14);

	tempMETs[10].setCoefficient(1,12);

	//Twelfth Term
	tempIrrTensors[11].addUpperIndex(sourceReducibleTensor.getIndexNameAt(2));

	Delta delta15(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(0));
	Delta delta16(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[11].addIrreducibleTensor(tempIrrTensors[11]);
	tempMETs[11].addDelta(delta15);
	tempMETs[11].addDelta(delta16);

	tempMETs[11].setCoefficient(1,12);

	//Thirteenth Term
	tempIrrTensors[12].addUpperIndex(sourceReducibleTensor.getIndexNameAt(2));

	Delta delta17(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(0));
	Delta delta18(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[12].addIrreducibleTensor(tempIrrTensors[12]);
	tempMETs[12].addDelta(delta17);
	tempMETs[12].addDelta(delta18);

	tempMETs[12].setCoefficient(-1,12);
	
	//Add terms to Expression
	for (auto& tempMET : tempMETs) {
		addTerm(tempMET);
	}
}

void MathExpression::setSubstitutionFor5Index3Bar(Tensor& sourceReducibleTensor) {
	std::vector<MathExpressionTerm> tempMETs(13);
	std::vector<IrreducibleTensor> tempIrrTensors;

	for (int i = 0; i < 13; ++i) {
		IrreducibleTensor tempIrrTensor(false, 5);
		tempIrrTensors.push_back(tempIrrTensor);
	}

	//First Term
	for (int i = 0; i < 3; ++i) {
		tempIrrTensors[0].addLowerIndex(sourceReducibleTensor.getIndexNameAt(i));
	}
	for (int i = 0; i < 2; ++i) {
		tempIrrTensors[0].addUpperIndex(sourceReducibleTensor.getIndexNameAt(i+3));
	}


	tempMETs[0].addIrreducibleTensor(tempIrrTensors[0]);
	tempMETs[0].setCoefficient(1);

	//Second Term
	tempIrrTensors[1].addUpperIndex(sourceReducibleTensor.getIndexNameAt(4));
	
	tempIrrTensors[1].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));
	tempIrrTensors[1].addLowerIndex(sourceReducibleTensor.getIndexNameAt(2));

	Delta delta1(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(0));

	tempMETs[1].addIrreducibleTensor(tempIrrTensors[1]);
	tempMETs[1].addDelta(delta1);
	tempMETs[1].setCoefficient(1,2);

	//Third Term
	tempIrrTensors[2].addUpperIndex(sourceReducibleTensor.getIndexNameAt(4));

	tempIrrTensors[2].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));
	tempIrrTensors[2].addLowerIndex(sourceReducibleTensor.getIndexNameAt(2));

	Delta delta2(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[2].addIrreducibleTensor(tempIrrTensors[2]);
	tempMETs[2].addDelta(delta2);
	tempMETs[2].setCoefficient(-1,2);

	//Fourth Term
	tempIrrTensors[3].addUpperIndex(sourceReducibleTensor.getIndexNameAt(4));

	tempIrrTensors[3].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));
	tempIrrTensors[3].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));

	Delta delta3(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(2));

	tempMETs[3].addIrreducibleTensor(tempIrrTensors[3]);
	tempMETs[3].addDelta(delta3);
	tempMETs[3].setCoefficient(1,2);

	//Fifth Term
	tempIrrTensors[4].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));

	tempIrrTensors[4].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));
	tempIrrTensors[4].addLowerIndex(sourceReducibleTensor.getIndexNameAt(2));

	Delta delta4(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(0));

	tempMETs[4].addIrreducibleTensor(tempIrrTensors[4]);
	tempMETs[4].addDelta(delta4);
	tempMETs[4].setCoefficient(-1,2);

	//Sixth Term
	tempIrrTensors[5].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));

	tempIrrTensors[5].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));
	tempIrrTensors[5].addLowerIndex(sourceReducibleTensor.getIndexNameAt(2));

	Delta delta5(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[5].addIrreducibleTensor(tempIrrTensors[5]);
	tempMETs[5].addDelta(delta5);
	tempMETs[5].setCoefficient(1,2);

	//Seventh Term
	tempIrrTensors[6].addUpperIndex(sourceReducibleTensor.getIndexNameAt(3));

	tempIrrTensors[6].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));
	tempIrrTensors[6].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));

	Delta delta6(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(2));

	tempMETs[6].addIrreducibleTensor(tempIrrTensors[6]);
	tempMETs[6].addDelta(delta6);
	tempMETs[6].setCoefficient(-1,2);

	//Eighth Term
	tempIrrTensors[7].addLowerIndex(sourceReducibleTensor.getIndexNameAt(2));

	Delta delta7(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(0));
	Delta delta8(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[7].addIrreducibleTensor(tempIrrTensors[7]);
	tempMETs[7].addDelta(delta7);
	tempMETs[7].addDelta(delta8);

	tempMETs[7].setCoefficient(1,12);

	//Ninth Term
	tempIrrTensors[8].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));

	Delta delta9(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(0));
	Delta delta10(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(2));

	tempMETs[8].addIrreducibleTensor(tempIrrTensors[8]);
	tempMETs[8].addDelta(delta9);
	tempMETs[8].addDelta(delta10);

	tempMETs[8].setCoefficient(-1,12);

	//Tenth Term
	tempIrrTensors[9].addLowerIndex(sourceReducibleTensor.getIndexNameAt(2));

	Delta delta11(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(1));
	Delta delta12(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(0));

	tempMETs[9].addIrreducibleTensor(tempIrrTensors[9]);
	tempMETs[9].addDelta(delta11);
	tempMETs[9].addDelta(delta12);

	tempMETs[9].setCoefficient(-1,12);

	//Eleventh Term
	//Maybe error
	//tempIrrTensors[10].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));
	tempIrrTensors[10].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));

	Delta delta13(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(1));
	Delta delta14(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(2));

	tempMETs[10].addIrreducibleTensor(tempIrrTensors[10]);
	tempMETs[10].addDelta(delta13);
	tempMETs[10].addDelta(delta14);

	tempMETs[10].setCoefficient(1,12);

	//Twelfth Term
	tempIrrTensors[11].addLowerIndex(sourceReducibleTensor.getIndexNameAt(1));

	Delta delta15(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(2));
	Delta delta16(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(0));

	tempMETs[11].addIrreducibleTensor(tempIrrTensors[11]);
	tempMETs[11].addDelta(delta15);
	tempMETs[11].addDelta(delta16);

	tempMETs[11].setCoefficient(1,12);

	//Thirteenth Term
	tempIrrTensors[12].addLowerIndex(sourceReducibleTensor.getIndexNameAt(0));

	Delta delta17(sourceReducibleTensor.getIndexNameAt(3), sourceReducibleTensor.getIndexNameAt(2));
	Delta delta18(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(1));

	tempMETs[12].addIrreducibleTensor(tempIrrTensors[12]);
	tempMETs[12].addDelta(delta17);
	tempMETs[12].addDelta(delta18);

	tempMETs[12].setCoefficient(-1,12);



	//Add terms to Expression
	for (auto& tempMET : tempMETs) {
		addTerm(tempMET);
	}
}


void MathExpression::setSubstitutionFor5Index4Bar(Tensor& sourceReducibleTensor, std::string& latestName) {
	std::vector<MathExpressionTerm> tempMETs(5);
	std::vector<IrreducibleTensor> tempIrrTensors;
	std::vector<LeviCivita> tempLevis;
	IrreducibleTensor tempSymIrrTensor(true, 5);
	tempIrrTensors.push_back(tempSymIrrTensor);

	for (int i = 0; i < 4; ++i) {
		IrreducibleTensor tempIrrTensor(false, 5);
		tempIrrTensors.push_back(tempIrrTensor);
	}
	for (int i = 0; i < 5; ++i) {
		LeviCivita tempLevi(false);
		tempLevis.push_back(tempLevi);
	}

	//First Term
	for (int i = 0; i < 4; ++i) {
		tempLevis[0].addIndex(sourceReducibleTensor.getIndexNameAt(i));
	}
	latestName = getNextName(latestName);
	tempLevis[0].addIndex(latestName);
	tempIrrTensors[0].addUpperIndex(latestName);
	tempIrrTensors[0].addUpperIndex(sourceReducibleTensor.getIndexNameAt(4));

	tempMETs[0].addIrreducibleTensor(tempIrrTensors[0]);
	tempMETs[0].addLeviCivita(tempLevis[0]);
	tempMETs[0].setCoefficient(1);
	std::string copyOfFirstNameObtained(latestName);

	//Second Term
	latestName = getNextName(latestName);
	tempIrrTensors[1].addUpperIndex(copyOfFirstNameObtained);
	tempIrrTensors[1].addUpperIndex(latestName);

	Delta delta1(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(0));
	for (int i = 0; i < 3; ++i) {
		tempLevis[1].addIndex(sourceReducibleTensor.getIndexNameAt(i + 1));
	}
	tempLevis[1].addIndex(copyOfFirstNameObtained);
	tempLevis[1].addIndex(latestName);

	tempMETs[1].addIrreducibleTensor(tempIrrTensors[1]);
	tempMETs[1].addDelta(delta1);
	tempMETs[1].addLeviCivita(tempLevis[1]);

	tempMETs[1].setCoefficient(1,2);

	//Third Term
	tempIrrTensors[2].addUpperIndex(copyOfFirstNameObtained);
	tempIrrTensors[2].addUpperIndex(latestName);

	Delta delta2(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(1));
	tempLevis[2].addIndex(sourceReducibleTensor.getIndexNameAt(0));
	tempLevis[2].addIndex(sourceReducibleTensor.getIndexNameAt(2));
	tempLevis[2].addIndex(sourceReducibleTensor.getIndexNameAt(3));


	tempLevis[2].addIndex(copyOfFirstNameObtained);
	tempLevis[2].addIndex(latestName);

	tempMETs[2].addIrreducibleTensor(tempIrrTensors[2]);
	tempMETs[2].addDelta(delta2);
	tempMETs[2].addLeviCivita(tempLevis[2]);

	tempMETs[2].setCoefficient(-1,2);

	//Fourth Term
	tempIrrTensors[3].addUpperIndex(copyOfFirstNameObtained);
	tempIrrTensors[3].addUpperIndex(latestName);

	Delta delta3(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(2));
	tempLevis[3].addIndex(sourceReducibleTensor.getIndexNameAt(0));
	tempLevis[3].addIndex(sourceReducibleTensor.getIndexNameAt(1));
	tempLevis[3].addIndex(sourceReducibleTensor.getIndexNameAt(3));


	tempLevis[3].addIndex(copyOfFirstNameObtained);
	tempLevis[3].addIndex(latestName);

	tempMETs[3].addIrreducibleTensor(tempIrrTensors[3]);
	tempMETs[3].addDelta(delta3);
	tempMETs[3].addLeviCivita(tempLevis[3]);

	tempMETs[3].setCoefficient(1,2);

	//Fifth Term
	tempIrrTensors[4].addUpperIndex(copyOfFirstNameObtained);
	tempIrrTensors[4].addUpperIndex(latestName);

	Delta delta4(sourceReducibleTensor.getIndexNameAt(4), sourceReducibleTensor.getIndexNameAt(3));
	tempLevis[4].addIndex(sourceReducibleTensor.getIndexNameAt(0));
	tempLevis[4].addIndex(sourceReducibleTensor.getIndexNameAt(1));
	tempLevis[4].addIndex(sourceReducibleTensor.getIndexNameAt(2));


	tempLevis[4].addIndex(copyOfFirstNameObtained);
	tempLevis[4].addIndex(latestName);

	tempMETs[4].addIrreducibleTensor(tempIrrTensors[4]);
	tempMETs[4].addDelta(delta4);
	tempMETs[4].addLeviCivita(tempLevis[4]);

	tempMETs[4].setCoefficient(-1,2);

	//Add terms to Expression
	for (auto& tempMET : tempMETs) {
		addTerm(tempMET);
	}
}

void MathExpression::setSubstitutionFor5Index5Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 5);
	LeviCivita tempLevi(false);
	for (int i = 0; i < 5; ++i) {
		tempLevi.addIndex(sourceReducibleTensor.getIndexNameAt(i));
	}
	tempIrrTensor.setBarred(true);
	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.addLeviCivita(tempLevi);
	tempMET.setCoefficient(1);

	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor5Index1Duplicate1Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 5);

	TensorTerm shiftedTensorTerm = getTensorTermWithDuplicatedIndicesToLeft(sourceReducibleTensor);
	Tensor shiftedTensor(shiftedTensorTerm.getTensor(0));

	tempIrrTensor.addUpperIndex(shiftedTensor.getIndexNameAt(2));
	tempIrrTensor.addUpperIndex(shiftedTensor.getIndexNameAt(3));
	tempIrrTensor.addUpperIndex(shiftedTensor.getIndexNameAt(4));


	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.setCoefficient(shiftedTensorTerm.getCoefficient());
	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor5Index1Duplicate2Bar(Tensor& sourceReducibleTensor) {
	std::vector<MathExpressionTerm> tempMETs(3);
	std::vector<Delta> tempDeltas(2);
	std::vector<IrreducibleTensor> tempIrreducibleTensors;

	for (int i = 0; i < 3; ++i) {
		IrreducibleTensor t(false, 5);
		tempIrreducibleTensors.push_back(t);
	}

	TensorTerm shiftedTensorTerm = getTensorTermWithDuplicatedIndicesToLeft(sourceReducibleTensor);
	Tensor shiftedTensor(shiftedTensorTerm.getTensor(0));

	//Term 1
	tempIrreducibleTensors[0].addUpperIndex(shiftedTensor.getIndexNameAt(3));
	tempIrreducibleTensors[0].addUpperIndex(shiftedTensor.getIndexNameAt(4));
	tempIrreducibleTensors[0].addLowerIndex(shiftedTensor.getIndexNameAt(2));


	tempMETs[0].addIrreducibleTensor(tempIrreducibleTensors[0]);

	//Term 2
	tempDeltas[0].setIndices(shiftedTensor.getIndexNameAt(4), shiftedTensor.getIndexNameAt(2));
	tempMETs[1].addDelta(tempDeltas[0]);
	tempIrreducibleTensors[1].addUpperIndex(shiftedTensor.getIndexNameAt(3));
	tempMETs[1].addIrreducibleTensor(tempIrreducibleTensors[1]);
	tempMETs[1].setCoefficient(1, 4);
	
	//Term 3
	tempDeltas[1].setIndices(shiftedTensor.getIndexNameAt(3), shiftedTensor.getIndexNameAt(2));
	tempMETs[2].addDelta(tempDeltas[1]);
	tempIrreducibleTensors[2].addUpperIndex(shiftedTensor.getIndexNameAt(4));
	tempMETs[2].addIrreducibleTensor(tempIrreducibleTensors[2]);
	tempMETs[2].setCoefficient(-1, 4);

	//Add terms
	for (auto& met : tempMETs) {
		addTerm(met);
	}

	multiplyByCoefficient(shiftedTensorTerm.getCoefficient() * -1);
}

void MathExpression::setSubstitutionFor5Index1Duplicate3Bar(Tensor& sourceReducibleTensor) {
	std::vector<MathExpressionTerm> tempMETs(3);
	std::vector<Delta> tempDeltas(2);
	std::vector<IrreducibleTensor> tempIrreducibleTensors;

	for (int i = 0; i < 3; ++i) {
		IrreducibleTensor t(false, 5);
		tempIrreducibleTensors.push_back(t);
	}

	TensorTerm shiftedTensorTerm = getTensorTermWithDuplicatedIndicesToLeft(sourceReducibleTensor);
	Tensor shiftedTensor(shiftedTensorTerm.getTensor(0));

	//Term 1
	tempIrreducibleTensors[0].addUpperIndex(shiftedTensor.getIndexNameAt(4));
	tempIrreducibleTensors[0].addLowerIndex(shiftedTensor.getIndexNameAt(2));
	tempIrreducibleTensors[0].addLowerIndex(shiftedTensor.getIndexNameAt(3));


	tempMETs[0].addIrreducibleTensor(tempIrreducibleTensors[0]);

	//Term 2
	tempDeltas[0].setIndices(shiftedTensor.getIndexNameAt(4), shiftedTensor.getIndexNameAt(3));
	tempMETs[1].addDelta(tempDeltas[0]);
	tempIrreducibleTensors[1].addLowerIndex(shiftedTensor.getIndexNameAt(2));
	tempMETs[1].addIrreducibleTensor(tempIrreducibleTensors[1]);
	tempMETs[1].setCoefficient(1, 4);

	//Term 3
	tempDeltas[1].setIndices(shiftedTensor.getIndexNameAt(4), shiftedTensor.getIndexNameAt(2));
	tempMETs[2].addDelta(tempDeltas[1]);
	tempIrreducibleTensors[2].addLowerIndex(shiftedTensor.getIndexNameAt(3));
	tempMETs[2].addIrreducibleTensor(tempIrreducibleTensors[2]);
	tempMETs[2].setCoefficient(-1, 4);

	//Add terms
	for (auto& met : tempMETs) {
		addTerm(met);
	}

	multiplyByCoefficient(shiftedTensorTerm.getCoefficient());
}

void MathExpression::setSubstitutionFor5Index1Duplicate4Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 5);

	TensorTerm shiftedTensorTerm = getTensorTermWithDuplicatedIndicesToLeft(sourceReducibleTensor);
	Tensor shiftedTensor(shiftedTensorTerm.getTensor(0));

	tempIrrTensor.addLowerIndex(shiftedTensor.getIndexNameAt(2));
	tempIrrTensor.addLowerIndex(shiftedTensor.getIndexNameAt(3));
	tempIrrTensor.addLowerIndex(shiftedTensor.getIndexNameAt(4));


	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.setCoefficient(shiftedTensorTerm.getCoefficient() * -1);
	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor5Index2Duplicate2Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 5);

	TensorTerm shiftedTensorTerm = getTensorTermWithDuplicatedIndicesToLeft(sourceReducibleTensor);
	Tensor shiftedTensor(shiftedTensorTerm.getTensor(0));

	tempIrrTensor.addUpperIndex(shiftedTensor.getIndexNameAt(4));

	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.setCoefficient(shiftedTensorTerm.getCoefficient());
	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor5Index2Duplicate3Bar(Tensor& sourceReducibleTensor) {
	MathExpressionTerm tempMET;
	IrreducibleTensor tempIrrTensor(false, 5);

	TensorTerm shiftedTensorTerm = getTensorTermWithDuplicatedIndicesToLeft(sourceReducibleTensor);
	Tensor shiftedTensor(shiftedTensorTerm.getTensor(0));

	tempIrrTensor.addLowerIndex(shiftedTensor.getIndexNameAt(4));

	tempMET.addIrreducibleTensor(tempIrrTensor);
	tempMET.setCoefficient(shiftedTensorTerm.getCoefficient());
	addTerm(tempMET);
}

void MathExpression::setSubstitutionFor5Index(Tensor& sourceReducibleTensor, std::string& latestName) {
	int numOfBarredIndices = sourceReducibleTensor.getNumberOfBarredIndices();
	int numOfDuplicatedIndices = sourceReducibleTensor.getNumberOfDuplicatedIndices();

	if (numOfDuplicatedIndices == 2) {
		if (numOfBarredIndices == 2) {
			setSubstitutionFor5Index2Duplicate2Bar(sourceReducibleTensor);
		}
		else if (numOfBarredIndices == 3) {
			setSubstitutionFor5Index2Duplicate3Bar(sourceReducibleTensor);
		}
	}
	else if (numOfDuplicatedIndices == 1) {
		if (numOfBarredIndices == 1) {
			setSubstitutionFor5Index1Duplicate1Bar(sourceReducibleTensor);
		}
		else if (numOfBarredIndices == 2) {
			setSubstitutionFor5Index1Duplicate2Bar(sourceReducibleTensor);
		}
		else if (numOfBarredIndices == 3) {
			setSubstitutionFor5Index1Duplicate3Bar(sourceReducibleTensor);
		}
		else if (numOfBarredIndices == 4) {
			setSubstitutionFor5Index1Duplicate4Bar(sourceReducibleTensor);
		}
	}
	else  if (numOfDuplicatedIndices == 0) {
		if (numOfBarredIndices == 0) {
			setSubstitutionFor5Index0Bar(sourceReducibleTensor);
		}
		else if (numOfBarredIndices == 1) {
			setSubstitutionFor5Index1Bar(sourceReducibleTensor, latestName);
		}
		else if (numOfBarredIndices == 2) {
			setSubstitutionFor5Index2Bar(sourceReducibleTensor);
		}
		else if (numOfBarredIndices == 3) {
			setSubstitutionFor5Index3Bar(sourceReducibleTensor);
		}
		else if (numOfBarredIndices == 4) {
			setSubstitutionFor5Index4Bar(sourceReducibleTensor, latestName);
		}
		else if (numOfBarredIndices == 5) {
			setSubstitutionFor5Index5Bar(sourceReducibleTensor);
		}
	}
	
}

void MathExpression::setSubstitutionFromReducibleTensor(Tensor& sourceReducibleTensor, std::string& latestName) {
	int numOfIndices = sourceReducibleTensor.getNumberOfIndices();
	if (numOfIndices == 1) {
		setSubstitutionFor1Index(sourceReducibleTensor);
	}
	else if (numOfIndices == 2) {
		setSubstitutionFor2Index(sourceReducibleTensor);
	}
	else if (numOfIndices == 3) {
		setSubstitutionFor3Index(sourceReducibleTensor, latestName);
	}
	else if (numOfIndices == 4) {
		setSubstitutionFor4Index(sourceReducibleTensor, latestName);
	}
	else if (numOfIndices == 5) {
		setSubstitutionFor5Index(sourceReducibleTensor, latestName);
	}
}

void MathExpression::setSubstitutionsFor5Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi) {
	MathExpressionTerm tempMET;
	tempMET.setCoefficient(120);
	addTerm(tempMET);
}

void MathExpression::setSubstitutionsFor4Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi) {
	MathExpressionTerm tempMET;
	Delta tempDelta(secondLevi.getNameAtLocation(4), firstLevi.getNameAtLocation(4));
	tempMET.setCoefficient(24);
	tempMET.addDelta(tempDelta);

	addTerm(tempMET);
}

void MathExpression::setSubstitutionsFor3Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi) {
	std::vector<MathExpressionTerm> tempMETs(2);
	std::vector<Delta> tempDeltas(4);

	//First Term
	tempDeltas[0].setIndices(secondLevi.getNameAtLocation(3), firstLevi.getNameAtLocation(3));
	tempDeltas[1].setIndices(secondLevi.getNameAtLocation(4), firstLevi.getNameAtLocation(4));
	tempMETs[0].addDelta(tempDeltas[0]);
	tempMETs[0].addDelta(tempDeltas[1]);
	tempMETs[0].setCoefficient(6);

	//Second Term
	tempDeltas[2].setIndices(secondLevi.getNameAtLocation(4), firstLevi.getNameAtLocation(3));
	tempDeltas[3].setIndices(secondLevi.getNameAtLocation(3), firstLevi.getNameAtLocation(4));
	tempMETs[1].addDelta(tempDeltas[2]);
	tempMETs[1].addDelta(tempDeltas[3]);
	tempMETs[1].setCoefficient(-6);

	//Add all terms to expression
	for (auto& MET : tempMETs) {
		addTerm(MET);
	}

}

void MathExpression::setSubstitutionsFor2Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi,const  MathExpression& predeterminedDeltaExpression) {
	(*this) = predeterminedDeltaExpression;
	renameDeltaExpressionBasedOnLevis(2, firstLevi, secondLevi, *this);
}

void MathExpression::setSubstitutionsFor1Match(const LeviCivita& firstLevi, const LeviCivita& secondLevi, const MathExpression& predeterminedDeltaExpression) {
	(*this) = predeterminedDeltaExpression;
	renameDeltaExpressionBasedOnLevis(1, firstLevi, secondLevi, *this);
}

void MathExpression::setSubstitutionsFor0Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi, const MathExpression& predeterminedDeltaExpression) {
	(*this) = predeterminedDeltaExpression;
	renameDeltaExpressionBasedOnLevis(0, firstLevi, secondLevi, *this);
}

void MathExpression::multiplyByCoefficient(int numerator, int denominator) {
	MathExpressionTerm tempTerm;
	tempTerm.setCoefficient(numerator, denominator);
	MathExpression coeffExp;
	coeffExp.addTerm(tempTerm);
	(*this) = expandMathExpressions(*this, coeffExp);
}

void MathExpression::multiplyByCoefficient(int regularInteger) {
	MathExpressionTerm tempTerm;
	tempTerm.setCoefficient(regularInteger);
	MathExpression coeffExp;
	coeffExp.addTerm(tempTerm);
	(*this) = expandMathExpressions(*this, coeffExp);
}

void MathExpression::multiplyByCoefficient(Coefficient inpCoefficient) {
	MathExpressionTerm tempTerm;
	tempTerm.setCoefficient(inpCoefficient);
	MathExpression coeffExp;
	coeffExp.addTerm(tempTerm);
	(*this) = expandMathExpressions(*this, coeffExp);
}



void MathExpression::setPredeterminedSubstitutionsFor2Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi) {
	std::vector<std::vector<Delta>> deltaMatrix;
	//Allocate matrix
	for (int i = 0; i < 3; ++i) {
		std::vector<Delta> tempVector(3);
		deltaMatrix.push_back(tempVector);
	}
	//Fill the matrix
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			//Sets the appropriate name from the Levis. "+2" because first 2 indices are matched.
			deltaMatrix[i][j].setIndices(secondLevi.getNameAtLocation(j+2), firstLevi.getNameAtLocation(i + 2));
		}
	}
	//Find the determinant
	(*this) = determinantOfDeltaMatrix(deltaMatrix);
	this->multiplyByCoefficient(2);
	
}

void MathExpression::setPredeterminedSubstitutionsFor1Match(const LeviCivita& firstLevi, const LeviCivita& secondLevi) {
	std::vector<std::vector<Delta>> deltaMatrix;
	//Allocate matrix
	for (int i = 0; i < 4; ++i) {
		std::vector<Delta> tempVector(4);
		deltaMatrix.push_back(tempVector);
	}
	//Fill the matrix
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			//Sets the appropriate name from the Levis. "+1" because first index is matched.
			deltaMatrix[i][j].setIndices(secondLevi.getNameAtLocation(j + 1), firstLevi.getNameAtLocation(i + 1));
		}
	}
	//Find the determinant
	(*this) = determinantOfDeltaMatrix(deltaMatrix);
}

void MathExpression::setPredeterminedSubstitutionsFor0Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi) {
	std::vector<std::vector<Delta>> deltaMatrix;
	//Allocate matrix
	for (int i = 0; i < 5; ++i) {
		std::vector<Delta> tempVector(5);
		deltaMatrix.push_back(tempVector);
	}
	//Fill the matrix
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			deltaMatrix[i][j].setIndices(secondLevi.getNameAtLocation(j), firstLevi.getNameAtLocation(i));
		}
	}
	//Find the determinant
	(*this) = determinantOfDeltaMatrix(deltaMatrix);
}



MathExpression MathExpression::operator+(const MathExpression& otherExpression) const {
	MathExpression result(*this);
	for (auto& term : otherExpression.mathExpressionTerms) {
		result.mathExpressionTerms.push_back(term);
	}
	return result;
}

MathExpression& MathExpression::operator+=(const MathExpression& otherExpression) {
	mathExpressionTerms.reserve(mathExpressionTerms.size() + otherExpression.getSize());
	mathExpressionTerms.insert(mathExpressionTerms.end(), otherExpression.mathExpressionTerms.begin(), otherExpression.mathExpressionTerms.end());
	return *this;
}

void MathExpression::appendWithoutReservation(const MathExpression& otherExpression) {
	mathExpressionTerms.insert(mathExpressionTerms.end(), otherExpression.mathExpressionTerms.begin(), otherExpression.mathExpressionTerms.end());
}



void MathExpression::expandExpressionWithTermAtLocation(int location, const MathExpression& mathExpression) {
	MathExpression originalTermExpression;
	originalTermExpression.addTerm(mathExpressionTerms[location]);

	MathExpression expandedExpression = expandMathExpressions(originalTermExpression, mathExpression);
	
	//Remove the original term as it is multiplied into the expanded expression already
	mathExpressionTerms.erase(mathExpressionTerms.begin() + location);


	//Add the expanded terms to the original term
	(*this) += expandedExpression;

}


void MathExpression::setSubstitutionForLeviCivita(int numOfTermsInCommon, const LeviCivita& firstLevi, const LeviCivita& secondLevi, const MathExpression& pred2Match, const MathExpression& pred1Match, const MathExpression& pred0Match) {
	if (numOfTermsInCommon == 5) {
		setSubstitutionsFor5Matches(firstLevi, secondLevi);
	}
	else if (numOfTermsInCommon == 4) {
		setSubstitutionsFor4Matches(firstLevi, secondLevi);
	}
	else if (numOfTermsInCommon == 3) {
		setSubstitutionsFor3Matches(firstLevi, secondLevi);
	}
	else if (numOfTermsInCommon == 2) {
		setSubstitutionsFor2Matches(firstLevi, secondLevi, pred2Match);
	}
	else if (numOfTermsInCommon == 1) {
		setSubstitutionsFor1Match(firstLevi, secondLevi, pred1Match);
	}
	else if (numOfTermsInCommon == 0) {
		setSubstitutionsFor0Matches(firstLevi, secondLevi, pred0Match);
	}
}

void MathExpression::expandAllLeviCivitas(const MathExpression& pred2Match, const MathExpression& pred1Match, const MathExpression& pred0Match) {

	simplifyExpressionByDeltas();
	simplifyExpressionBySymmetricAsymmetricProperty();
	int debug = 0;
	//Expand the levis in every term
	for (int i = 0; i < mathExpressionTerms.size(); ++i) {
		if (mathExpressionTerms[i].getNumberOfLeviCivitas() > 1) {
			if (mathExpressionTerms[i].getNumberOfLeviCivitas() >= 3) {
				mathExpressionTerms[i].groupLevisBySimilarIndices();
			}


			MathExpression tempLeviExpansion = mathExpressionTerms[i].expandPairOfLeviCivitas(pred2Match, pred1Match, pred0Match);
			mathExpressionTerms[i].deletePairOfLeviCivitas();


			while (mathExpressionTerms[i].getNumberOfLeviCivitas() > 1) {
				MathExpression currentLeviExpansion = mathExpressionTerms[i].expandPairOfLeviCivitas(pred2Match, pred1Match, pred0Match);
				mathExpressionTerms[i].deletePairOfLeviCivitas();
				tempLeviExpansion = expandMathExpressions(tempLeviExpansion, currentLeviExpansion);
			}
			//tempLeviExpansion.sumOverAllDeltas();


			MathExpression originalTermExpression;
			originalTermExpression.addTerm(mathExpressionTerms[i]);
			MathExpression expandedExpression = expandMathExpressions(originalTermExpression, tempLeviExpansion);
			//Remove the original term as it is multiplied into the expanded expression already
			mathExpressionTerms[i].setCoefficient(0);

						
			expandedExpression.simplifyExpressionByDeltas();
			expandedExpression.simplifyExpressionBySymmetricAsymmetricProperty();
			
			//(*this) += expandedExpression;
			
			appendWithoutReservation(expandedExpression);

		}
	}
	erase0Terms();
}

void MathExpression::renameDeltasOfTerms(std::vector<std::pair<std::string, std::string>>& renameMap) {
	for (auto& MET : mathExpressionTerms) {
		MET.renameDeltas(renameMap);
	}
}

void MathExpression::reserveSpaceFor(long long int numberOfElements) {
	mathExpressionTerms.reserve(numberOfElements);
}


void MathExpression::sortIrreducibleTensorsOfAllTerms() {
	for (auto& MET : mathExpressionTerms) {
		MET.sortIrreducibleTensors();
	}
}

void MathExpression::mergeTermAtLocationWithGivenTerm(int location, const MathExpressionTerm& otherTerm) {
	mathExpressionTerms[location].mergeTerms(otherTerm);
}


void MathExpression::reorderIndicesOfIrreducibleTensorsAndLeviCivitasOfAllTerms() {
	for (auto& MET : mathExpressionTerms) {
		MET.reorderIndicesOfIrreducibleTensorsAndLeviCivitas();
	}
}

void MathExpression::simplifyExpressionByRenaming() {
	sortIrreducibleTensorsOfAllTerms();
	reorderIndicesOfIrreducibleTensorsAndLeviCivitasOfAllTerms();


	MathExpression finalResult;
	for (int i = 0; i < mathExpressionTerms.size(); ++i) {
		bool oneSuccessfulMatch = false;
		//Check all terms in simplified expression that has been generated so far
		for (int j = 0; j < finalResult.getSize(); ++j) {

			if (areMathExpressionTermsSameStructure(finalResult.mathExpressionTerms[j], mathExpressionTerms[i])) {
				MathExpressionTerm renamedTerm;
				//Attempt to rename the indices
				bool successfulRename = performRenameIfValidIncludingPermutationsOnIrreducibleTensors(finalResult.mathExpressionTerms[j], mathExpressionTerms[i], renamedTerm);

				if (successfulRename) {
					renamedTerm.reorderIndicesOfIrreducibleTensorsAndLeviCivitas();

					//Technically, a successful rename, once ordered, should always be identical. But just to be safe
					if (areMathExpressionTermsIdentical(finalResult.mathExpressionTerms[j],renamedTerm)) {
						finalResult.mathExpressionTerms[j].addMathExpressionTermCoefficientToThisTerm(renamedTerm);
						oneSuccessfulMatch = true;
						break;
					}
					//DEBUG
					else {
						//cout << "ERROR" << endl;
					}
				}
			}
		}
		if (!oneSuccessfulMatch) {
			finalResult.addTerm(mathExpressionTerms[i]);
		}
	}
	(*this) = finalResult;
	erase0Terms();
}


void MathExpression::erase0Terms() {
	mathExpressionTerms.erase(std::remove_if(mathExpressionTerms.begin(), mathExpressionTerms.end(), isCoefficientZero), mathExpressionTerms.end());
}

void MathExpression::simplifyExpressionBySymmetricAsymmetricProperty() {
	//for (int i = 0; i < mathExpressionTerms.size(); ++i) {
	//	if (!mathExpressionTerms[i].simplifyTermBySymmetricAsymmetricProperty()) {
	//		mathExpressionTerms.erase(mathExpressionTerms.begin() + i);
	//		--i;
	//	}
	//}

	//Possible efficient way
	for (auto& MET : mathExpressionTerms) {
		MET.simplifyTermBySymmetricAsymmetricProperty();
	}
	mathExpressionTerms.erase(std::remove_if(mathExpressionTerms.begin(), mathExpressionTerms.end(), isCoefficientZero), mathExpressionTerms.end());
}

void MathExpression::normalizeAllIrreducibleTensors() {
	for (auto& MET : mathExpressionTerms) {
		while (MET.getNumberOfIrreducibleTensors() != 0) {
			MET.substituteNormalizedTensor(0);
		}
	}
}




//Non-member function definitions
MathExpression determinantOfDeltaMatrix(std::vector<std::vector<Delta>> deltaMatrix) {
	MathExpression finalResult;
	//Assuming square matrix always
	//Base case is determinant of 2x2 matrix
	if (deltaMatrix.size() == 2) {
		MathExpressionTerm tempTerm1, tempTerm2;
		tempTerm1.addDelta(deltaMatrix[0][0]);
		tempTerm1.addDelta(deltaMatrix[1][1]);
		tempTerm1.setCoefficient(1);

		tempTerm2.addDelta(deltaMatrix[0][1]);
		tempTerm2.addDelta(deltaMatrix[1][0]);
		tempTerm2.setCoefficient(-1);
		MathExpression resultExpression;
		resultExpression.addTerm(tempTerm1);
		resultExpression.addTerm(tempTerm2);
		return resultExpression;
	}
	else {
		for (int j = 0; j < deltaMatrix.size(); ++j) {
			MathExpressionTerm pivot;
			pivot.addDelta(deltaMatrix[0][j]);

			if (j % 2 != 0) {
				pivot.setCoefficient(-1);
			}

			MathExpression pivotExpression;
			pivotExpression.addTerm(pivot);

			std::vector<std::vector<Delta>> submatrix = getSubmatrix(deltaMatrix, 0, j);

			MathExpression determinantOfSubmatrix = determinantOfDeltaMatrix(submatrix);
			MathExpression expandedPivotExpression = expandMathExpressions(pivotExpression, determinantOfSubmatrix);
			finalResult = finalResult + expandedPivotExpression;
		}
		return finalResult;
	}
}

std::vector<std::vector<Delta>> getSubmatrix(const std::vector<std::vector<Delta>>& deltaMatrix, int skipRow, int skipColumn) {
	std::vector<std::vector<Delta>> submatrix;
	//Allocate matrix
	for (int i = 0; i < deltaMatrix.size()-1; ++i) {
		std::vector<Delta> tempVector(deltaMatrix.size() - 1);
		submatrix.push_back(tempVector);
	}
	//Fill the matrix
	int k = 0;
	for (int i = 0; i < deltaMatrix.size(); ++i) {
		int l = 0;
		if (i != skipRow) {
			for (int j = 0; j < deltaMatrix.size(); ++j) {
				if (j != skipColumn) {
					submatrix[k][l] = deltaMatrix[i][j];
					++l;
				}
			}
			++k;
		}
	}
	return submatrix;
}

//*************************Phase 2********************************
void MathExpression::printPhase2() const {
	if (mathExpressionTerms.empty()) {
		cout << 0 << endl;
	}
	for (auto& MET : mathExpressionTerms) {
		if (MET.getCoefficient() > 0) {
			cout << "+";
		}
		MET.printPhase2();
		cout << endl;
	}
}

MathExpression MathExpression::expandGammaAndInitialTensor(const MathExpressionTerm& originalTerm) {
	// We perform all permutations using binary counting. Start with calculating the end point...
	// Number of bits in the end = number of subscripts.
	MathExpression resultExpression;
	std::vector<std::string> allIndexNames = originalTerm.getInitialIndicesFromGamma();
	unsigned int numberOfIndices = allIndexNames.size();
	MathExpressionTerm termToDuplicate(originalTerm);
	termToDuplicate.fillBbtIndicesFromGamma();

	uint32_t end = 1 << numberOfIndices;
	for (uint32_t i = 0; i < end; i++) {

		MathExpressionTerm currentGeneratedTerm(termToDuplicate);
		uint32_t mask = 1;

		int locationOfIndexToChoose = numberOfIndices - 1;
		for (uint32_t j = 0; j < numberOfIndices; j++) {
			uint32_t bar_state = (i & mask) >> j;

			currentGeneratedTerm.fillBbtDaggersAndReducibleTensorBarsBasedOnBinaryMarker(allIndexNames[locationOfIndexToChoose], bar_state);
			locationOfIndexToChoose--;
			mask = mask << 1;
		}
		currentGeneratedTerm.setGammeIsNull(true);
		resultExpression.addTerm(currentGeneratedTerm);
	}
	return resultExpression;
}

void MathExpression::shiftAllRightBbtChainBtOperatorsToLeft() {
	for (int i = 0; i < mathExpressionTerms.size(); ++i) {
		appendWithoutReservation(mathExpressionTerms[i].shiftRightBbtChainBtOperatorsToLeft());
	}
}

void MathExpression::simplifyExpressionWithReducibleTensorsByDeltas() {
	for (auto& MET : mathExpressionTerms) {
		MET.simplifyTermWithReducibleTensorsByDeltas();
	}
}

void MathExpression::simplifyExpressionByRenamingBbtAndReducibleTensors() {
	if (mathExpressionTerms.size() == 0) return;
	ProductResolver tempProductResolver;
	std::vector<TensorTerm> tempTensorTerms;
	for (auto& term : mathExpressionTerms) {
		tempTensorTerms.push_back(term.convertToTensorTermEquivalent());
	}
	tempProductResolver.setAllReducibleTensorTermsFromVector(tempTensorTerms);
	tempProductResolver.simplifyReducibleTensorTerms();

	std::vector<TensorTerm> simplifiedTensorTerms = tempProductResolver.getSimplifiedTensorTerms();
	
	std::vector<MathExpressionTerm> newMathExpressionTerms;
	for (auto& tensorTerm : simplifiedTensorTerms) {
		newMathExpressionTerms.push_back(tensorTerm.convertToMet(mathExpressionTerms[0]));
	}
	mathExpressionTerms = newMathExpressionTerms;
}

void MathExpression::reduceReducibleTensorsPhase2(const std::string& calculationWideLatestName) {
	MathExpression finalMathExpression;
	//For every MET
	for (int i = 0; i < mathExpressionTerms.size(); ++i) {
		std::string latestName(calculationWideLatestName);
		MathExpression currentMetExpression;
		//Initial term containing fab/spinors/etc. info
		MathExpressionTerm initialTerm(mathExpressionTerms[i]);
		initialTerm.setCoefficient(1);
		//Clear reducible tensors
		initialTerm.setReducibleTensors(std::vector<Tensor>());
		currentMetExpression.addTerm(initialTerm);

		//For every reducible tensor in that met
		for (int j = 0; j < mathExpressionTerms[i].getNumberOfReducibleTensors(); ++j) {
			MathExpression tempME;
			Tensor currentTensor = mathExpressionTerms[i].getReducibleTensorAt(j);
			tempME.setSubstitutionFromReducibleTensor(currentTensor, latestName);
			
			////DEBUG
			//cout << "substituting: " << endl;
			//tempME.print();
			//cout << endl;
		
			currentMetExpression = expandMathExpressions(currentMetExpression, tempME);
		}
		finalMathExpression += currentMetExpression;
	}
	(*this) = finalMathExpression;
}

TensorTerm MathExpression::getTensorTermWithDuplicatedIndicesToLeft(const Tensor& sourceReducibleTensor) {
	Tensor tempTensor(sourceReducibleTensor);
	TensorTerm resultTensorTerm;
	resultTensorTerm.addTensor(tempTensor);
	std::vector<std::string> duplicatedIndices = tempTensor.getDuplicatedIndices();
	for (auto& dupIndex : duplicatedIndices) {
		resultTensorTerm.shiftDuplicatedIndexToLeft(dupIndex, 0);
	}
	return resultTensorTerm;
}

void MathExpression::reduceRightBbtChainsByEvaluation() {
	for (auto& met : mathExpressionTerms) {
		met.reduceRightBbtChainByEvaluation();
	}
}

void MathExpression::evaluateChargeConjugates() {
	for (auto& met : mathExpressionTerms) {
		met.evaluateChargeConjugate();
	}
}






