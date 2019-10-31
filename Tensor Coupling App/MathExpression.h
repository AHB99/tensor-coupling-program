#ifndef MATH_EXPRESSION_H
#define MATH_EXPRESSION_H

#include <iostream>
#include <vector>
#include <string>

#include "MathExpressionTerm.h"

using std::cout;
using std::cin;
using std::endl;

class Tensor;

class MathExpression {
public:
	void print() const;
	void printLatex() const;


	//For debugging
	void inputByUser();

	int getSize() const {
		return mathExpressionTerms.size();
	}

	MathExpressionTerm getTermAtLocation(int location) const;

	void addTerm(const MathExpressionTerm& met);
	MathExpression operator+(const MathExpression& otherExpression) const;
	MathExpression& operator+=(const MathExpression& otherExpression);
	void multiplyByCoefficient(int numerator, int denominator);
	void multiplyByCoefficient(int regularInteger);
	void multiplyByCoefficient(Coefficient inpCoefficient);


	
	void simplifyExpressionByDeltas();

	//Only checks summing and cancellation
	void simplifyExpressionByDeltasTillCancellationOnly();


	void expandAllLeviCivitas(const MathExpression& pred2Match, const MathExpression& pred1Match, const MathExpression& pred0Match);

	//Converts a reducible tensor to a Math Expression of Irreducible Tensors
	void setSubstitutionFromReducibleTensor(Tensor& sourceReducibleTensor, std::string& latestName);
	
	void setSubstitutionForLeviCivita(int numOfTermsInCommon, const LeviCivita& firstLevi, const LeviCivita& secondLevi, const MathExpression& pred2Match, const MathExpression& pred1Match, const MathExpression& pred0Match);

	//Leaves term multiplied with at coefficient 0. To be deleted.
	void expandExpressionWithTermAtLocation(int location, const MathExpression& mathExpression);

	void sortIrreducibleTensorsOfAllTerms();
	void reorderIndicesOfIrreducibleTensorsAndLeviCivitasOfAllTerms();

	//Assumes deltas and Levis are handled. Considers all permutations if needed
	void simplifyExpressionByRenaming();

	void simplifyExpressionBySymmetricAsymmetricProperty();

	void simplifyCoefficientsOfAllTerms();

	void mergeTermAtLocationWithGivenTerm(int location, const MathExpressionTerm& otherTerm);

	void normalizeAllIrreducibleTensors();

	void renameDeltasOfTerms(std::vector<std::pair<std::string, std::string>>& renameMap);

	//Predetermined Substitutions on start-up, i-r naming convention
	void setPredeterminedSubstitutionsFor2Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi);
	void setPredeterminedSubstitutionsFor1Match(const LeviCivita& firstLevi, const LeviCivita& secondLevi);
	void setPredeterminedSubstitutionsFor0Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi);

	//Efficiency function
	void reserveSpaceFor(long long int numberOfElements);

	void sumOverAllDeltas();

	void appendWithoutReservation(const MathExpression& otherExpression);

	//Erases terms with coefficient = 0
	void erase0Terms();

	//Phase 2
	void printPhase2() const;

	static MathExpression expandGammaAndInitialTensor(const MathExpressionTerm& met);
	void shiftAllRightBbtChainBtOperatorsToLeft();

	void simplifyExpressionWithReducibleTensorsByDeltas();

	void simplifyExpressionByRenamingBbtAndReducibleTensors();

	void reduceReducibleTensorsPhase2(const std::string& calculationWideLatestName);

	TensorTerm getTensorTermWithDuplicatedIndicesToLeft(const Tensor& sourceReducibleTensor);

	void setCoefficientOfTermAtLocation(int location, const Coefficient& inpCoeff) {
		mathExpressionTerms[location].setCoefficient(inpCoeff);
	}

	void reduceRightBbtChainsByEvaluation();
	void evaluateChargeConjugates();

	void simplifyExpressionByDeltasPhase2();


private:
	//The Expression Substitutions from REFERENCE.pdf

	void setSubstitutionFor1Index(Tensor& sourceReducibleTensor);
	void setSubstitutionFor2Index(Tensor& sourceReducibleTensor);
	void setSubstitutionFor3Index(Tensor& sourceReducibleTensor, std::string& latestName);
	void setSubstitutionFor4Index(Tensor& sourceReducibleTensor, std::string& latestName);
	void setSubstitutionFor5Index(Tensor& sourceReducibleTensor, std::string& latestName);

	//Field 1 Substitutions
	void setSubstitutionFor1Index0Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor1Index1Bar(Tensor& sourceReducibleTensor);


	//Field 2 Substitutions
	void setSubstitutionFor2Index0Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor2Index1Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor2Index2Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor2Index1Duplicate1Bar(Tensor& sourceReducibleTensor);

	//Field 3 Substitutions
	void setSubstitutionFor3Index0Bar(Tensor& sourceReducibleTensor, std::string& latestName);
	void setSubstitutionFor3Index1Bar(Tensor& sourceReducibleTensor, std::string& latestName);
	void setSubstitutionFor3Index2Bar(Tensor& sourceReducibleTensor, std::string& latestName);
	void setSubstitutionFor3Index3Bar(Tensor& sourceReducibleTensor, std::string& latestName);
	void setSubstitutionFor3Index1Duplicate1Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor3Index1Duplicate2Bar(Tensor& sourceReducibleTensor);


	//Field 4 Substitutions
	void setSubstitutionFor4Index0Bar(Tensor& sourceReducibleTensor, std::string& latestName);
	void setSubstitutionFor4Index1Bar(Tensor& sourceReducibleTensor, std::string& latestName);
	void setSubstitutionFor4Index2Bar(Tensor& sourceReducibleTensor, std::string& latestName);
	void setSubstitutionFor4Index3Bar(Tensor& sourceReducibleTensor, std::string& latestName);
	void setSubstitutionFor4Index4Bar(Tensor& sourceReducibleTensor, std::string& latestName);
	void setSubstitutionFor4Index1Duplicate1Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor4Index1Duplicate2Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor4Index1Duplicate3Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor4Index2Duplicate2Bar(Tensor& sourceReducibleTensor);


	//Field 5 Substitutions
	void setSubstitutionFor5Index0Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor5Index1Bar(Tensor& sourceReducibleTensor, std::string& latestName);
	void setSubstitutionFor5Index2Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor5Index3Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor5Index4Bar(Tensor& sourceReducibleTensor, std::string& latestName);
	void setSubstitutionFor5Index5Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor5Index1Duplicate1Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor5Index1Duplicate2Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor5Index1Duplicate3Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor5Index1Duplicate4Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor5Index2Duplicate2Bar(Tensor& sourceReducibleTensor);
	void setSubstitutionFor5Index2Duplicate3Bar(Tensor& sourceReducibleTensor);


	//Levi-Civita (with 5 indices) Substitutions
	void setSubstitutionsFor5Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi);
	void setSubstitutionsFor4Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi);
	void setSubstitutionsFor3Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi);
	void setSubstitutionsFor2Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi, const MathExpression& predeterminedDeltaExpression);
	void setSubstitutionsFor1Match(const LeviCivita& firstLevi, const LeviCivita& secondLevi, const MathExpression& predeterminedDeltaExpression);
	void setSubstitutionsFor0Matches(const LeviCivita& firstLevi, const LeviCivita& secondLevi, const MathExpression& predeterminedDeltaExpression);

	



	std::vector<MathExpressionTerm> mathExpressionTerms;
};

//Determinant Finder
MathExpression determinantOfDeltaMatrix(std::vector<std::vector<Delta>> deltaMatrix);
std::vector<std::vector<Delta>> getSubmatrix(const std::vector<std::vector<Delta>>& deltaMatrix, int upperLeftCell, int lowerRightCell);

#endif