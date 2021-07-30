#ifndef MATH_EXPRESSION_TERM_H
#define MATH_EXPRESSION_TERM_H

#include <iostream>
#include <vector>
#include <string>

#include "IrreducibleTensor.h"
#include "Delta.h"
#include "LeviCivita.h"
#include "Coefficient.h"
#include "NormalizedTensor.h"

using std::cout;
using std::cin;
using std::endl;

class MathExpression;

class MathExpressionTerm {
public:
	MathExpressionTerm() = default;
	void print() const;
	void printLatex() const;


	//For debugging
	void inputByUser();

	Coefficient getCoefficient() const {
		return coefficient;
	}

	void simplifyCoefficient();

	//Multiply the otherTerm with calling object, and reassign to calling object
	void mergeTerms(const MathExpressionTerm& otherTerm);


	//False if cancelled, leaves at 0 coefficient, to be deleted.
	bool simplifyTermByDeltas();

	//False if cancelled, leaves at 0 coefficient, to be deleted. Only checks summing and cancellation.
	bool simplifyTermByDeltasTillCancellationOnly();


	//For Expression Substitution
	void addIrreducibleTensor(const IrreducibleTensor& inpIrrTensor);
	void addDelta(const Delta& inpDelta);
	void addLeviCivita(const LeviCivita& inpLeviCivita);

	int getNumberOfLeviCivitas() const {
		return leviCevitas.size();
	}
	int getNumberOfIrreducibleTensors() const {
		return irreducibleTensors.size();
	}
	IrreducibleTensor getTensorAt(int location) const {
		return irreducibleTensors[location];
	}
	LeviCivita getLeviAt(int location) const {
		return leviCevitas[location];
	}
	//Assuming index exists in term. Returns -1 if found in Single Levi Civita
	int getTensorLocationOfFirstOccurenceOfIndex(const std::string& index) const;

	std::vector<std::string> getListOfAllIndices() const;

	void setCoefficient(int numerator, int denominator);
	void setCoefficient(int regularInteger);
	void setCoefficient(Coefficient otherCoefficient);

	bool hasSameDeltas(const MathExpressionTerm& otherMET) const;

	//Assumes an even number of Levis, atleast 2. Returns the substituted expression
	MathExpression expandPairOfLeviCivitas(const MathExpression& pred2Match, const MathExpression& pred1Match, const MathExpression& pred0Match);

	//Assumes atleast 2 alraedy exist
	void deletePairOfLeviCivitas();

	void sortIrreducibleTensors();

	void reorderIndicesOfIrreducibleTensorsAndLeviCivitas();

	void performRenamesOnIrreducibleTensors(const std::vector<std::pair<std::string, std::string>> renameMap);
	void performRenamesOnLeviCivita(const std::vector<std::pair<std::string, std::string>> renameMap);


	void addMathExpressionTermCoefficientToThisTerm(const MathExpressionTerm& otherMET);

	//False if cancelled, leaves at 0 coefficient, to be deleted.
	bool simplifyTermBySymmetricAsymmetricProperty();

	//Assumes sorted tensors, with no deltas or levis.
	bool isAmbiguous() const;

	//For renaming with ambiguous terms
	void getAllPermutationsOfAllAmbiguousZones(std::vector<MathExpressionTerm>& allIrreducibleTensorTermPermutations) const;

	//Replaces irreducible tensor at location with the corresponding normalized version and coefficient
	void substituteNormalizedTensor(int irreducibleTensorLocation);

	void renameDeltas(std::vector<std::pair<std::string, std::string>>& renameMap);

	void groupLevisBySimilarIndices();

	void sumOverDeltas();



private:
	//Helper for Levi grouping
	void moveLeviAtLocationToLeft(int location);



	//Sets coeff = 0 if true. Break simplification if coeff = 0
	bool checkForCancellationDeltas();

	void renameIrreducibleTensorsByDeltasIfPossible();

	//Multiplies coefficient by 5 (because SU(5)) if match found
	void solveMatchingDeltas();

	//Helpers for expandPairOfLeviCivitas
	void getNamesInCommonBetweenLevis(std::vector<std::string>& namesInCommon, int locationOfFirstLevi, int locationOfSecondLevi) const;
	void moveNamesInCommonBetweenLevisToLeft(std::vector<std::string>& namesInCommon, int locationOfFirstLevi, int locationOfSecondLevi);

	void addAllIrreducibleTensors(std::vector<IrreducibleTensor>& inputIrreducibleTensors);

	//Helpers for renaming with ambiguous terms
	void getAllAmbiguousZoneLocationsAndSizes(std::vector<std::pair<int, int>>& ambiguousZones) const;
	void getAllPermutationsOfLocationsForAllAmbiguousZones(std::vector<std::vector<std::vector<int>>>& locationPermutationsOfAllZones, std::vector<std::pair<int, int>>& ambiguousZones) const;

	//Helper for normalized substitution
	void addNormalizedTensorFromIrreducibleTensorAtLocation(int irreducibleTensorLocation);

	//Field 1 Normalized Substitutions
	void substituteNormalizedTensorFor1Index0Up1Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor1Index1Up0Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor1Index(int irreducibleTensorLocation);

	//Field 2 Normalized Substitutions
	void substituteNormalizedTensorFor2Index0Up0Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor2Index0Up2Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor2Index2Up0Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor2Index1Up1Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor2Index(int irreducibleTensorLocation);

	//Field 3 Normalized Substitutions
	void substituteNormalizedTensorFor3Index1Up0Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor3Index2Up0Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor3Index2Up1Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor3Index0Up1Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor3Index0Up2Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor3Index1Up2Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor3Index(int irreducibleTensorLocation);

	//Field 4 Normalized Substitutions
	void substituteNormalizedTensorFor4Index0Up0Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor4Index1Up0Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor4Index0Up1Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor4Index2Up0Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor4Index0Up2Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor4Index1Up1Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor4Index3Up1Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor4Index1Up3Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor4Index2Up2Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor4Index(int irreducibleTensorLocation);

	//Field 5 Normalized Substitutions
	void substituteNormalizedTensorFor5Index0Up0DownUnbarred(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor5Index0Up0DownBarred(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor5Index1Up0Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor5Index0Up1Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor5Index0Up2Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor5Index2Up0Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor5Index1Up2Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor5Index2Up1Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor5Index3Up2Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor5Index2Up3Down(int irreducibleTensorLocation);
	void substituteNormalizedTensorFor5Index(int irreducibleTensorLocation);

	//Member variables
	std::vector<IrreducibleTensor> irreducibleTensors;
	std::vector<Delta> deltas;
	std::vector<LeviCivita> leviCevitas;
	std::vector<NormalizedTensor> normalizedTensors;

	Coefficient coefficient;

	//Trial
};

#endif
