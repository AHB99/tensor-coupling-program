#ifndef  UTILITY_FUNCTIONS_H
#define UTILITY_FUNCTIONS_H

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <cmath>

#include "Tensor.h"
#include "TensorTerm.h"
#include "MathExpression.h"

using std::cout;
using std::cin;
using std::endl;

class IrreducibleTensor;
class Delta;


bool performRenameIfValid(TensorTerm& sourceTerm, TensorTerm& attemptTerm, TensorTerm& renamedTerm);

//Assuming Term Structure is found to be the same
bool performRenameIfValidIncludingPermutations(TensorTerm& sourceTerm, TensorTerm& attemptTerm, TensorTerm& renamedTerm);


bool isNameAlreadyAssigned(std::vector<std::pair<std::string, std::string>>& renameMap, std::string newName);

//Takes a pair representing a rename. First is oldName, second is newName.
bool checkZoneRenameIssue(TensorTerm& sourceTerm, TensorTerm& attemptTerm, std::pair<std::string, std::string> rename);

bool compareTensorsByNumberOfBarredIndices(const Tensor& lhs, const Tensor& rhs);
bool compareTensorsByNumberOfIndices(const Tensor& lhs, const Tensor& rhs);
bool compareTensorsByLabel(const Tensor& lhs, const Tensor& rhs);

std::vector<int> findAllPermutationsOfLocationsGivenSize(int size);

int factorial(int n);

//Assuming sorted and reordered terms
bool areTensorTermsSameStructure(TensorTerm& lhs, TensorTerm& rhs);

//Assuming sorted and reordered terms
bool areTensorTermsIdentical(TensorTerm& lhs, TensorTerm& rhs);

//----

MathExpression expandMathExpressions(const MathExpression& lhs, const MathExpression& rhs);

//In order to get a brand new name from a coupling for a new dummy index. 
std::string getNextName(std::string oldName);

int gcd(int a, int b);
int lcm(int a, int b);

void swapStrings(std::string& string1, std::string& string2);

bool compareIrreducibleTensorsByField(const IrreducibleTensor& lhs, const IrreducibleTensor& rhs);
bool compareIrreducibleTensorsBySymmetryState(const IrreducibleTensor& lhs, const IrreducibleTensor& rhs);
bool compareIrreducibleTensorsByBarState(const IrreducibleTensor& lhs, const IrreducibleTensor& rhs);
bool compareIrreducibleTensorsByNumberOfLowerIndices(const IrreducibleTensor& lhs, const IrreducibleTensor& rhs);
bool compareIrreducibleTensorsByNumberOfUpperIndices(const IrreducibleTensor& lhs, const IrreducibleTensor& rhs);
bool compareDeltasByUpperIndices(const Delta& lhs, const Delta& rhs);
bool compareDeltasByLowerIndices(const Delta& lhs, const Delta& rhs);

//Returns true if no issues
bool checkZoneRenameIssueInIrreducibleTensors(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, const std::pair<std::string, std::string>& rename);

bool isCoefficientZero(const MathExpressionTerm& MET);

bool compareIndexStrengths(const std::string& lhs, const std::string& rhs);

//Assuming same structure
bool performRenameIfValidOnIrreducibleTensors(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, MathExpressionTerm& renamedTerm);

//Assuming oldName exists
std::string getNewNameFromRenameMap(const std::string& oldName, std::vector<std::pair<std::string, std::string>> renameMap);

//Ignores coefficient. Assumes no deltas or Levis remaining.
bool areMathExpressionTermsIdentical(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm);

//Ignores coefficient. Assumes no deltas or Levis remaining. Checks if a rename could lead to identical.
bool areMathExpressionTermsSameStructure(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm);

//Assuming MathExpressionTerm Structure is found to be the same
bool performRenameIfValidIncludingPermutationsOnIrreducibleTensors(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, MathExpressionTerm& renamedTerm);

//Following i-r naming convention
void renameDeltaExpressionBasedOnLevis(int numOfMatches, const LeviCivita& firstLevi, const LeviCivita& secondLevi, MathExpression& mathExpressionToBeRenamed);

//******************************Phase 2***************************************

std::string getNextNameGivenTerm(const MathExpressionTerm& met);
long combination(int n, int r);
int reorderIndicesAntisymmetrically(std::vector<std::string>& vec);
bool areMathExpressionTermsSameStructurePhase2(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm);
bool areMathExpressionTermsIdenticalPhase2(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm);
bool performRenameIfValidPhase2(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, MathExpressionTerm& renamedTerm);
bool performRenameIfValidIncludingPermutationsPhase2(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, MathExpressionTerm& renamedTerm);
bool checkZoneRenameIssuePhase2(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, const std::pair<std::string, std::string>& rename);
bool trySingleRenamePhase2(const MathExpressionTerm& sourceTerm, const MathExpressionTerm& attemptTerm, const std::vector<std::string>& blindVector, std::vector<std::pair<std::string, std::string>>& renameMap, const std::string& trialRename);
#endif 