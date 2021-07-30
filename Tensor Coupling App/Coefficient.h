#ifndef COEFFICIENT_H
#define COEFFICIENT_H

#include <iostream>

using std::cout;
using std::cin;
using std::endl;


class Coefficient {
public:
	Coefficient() : nonRootNumerator(1), denominator(1), rootNumerator(1) {}
	Coefficient(int inpNonRootNumer, int inpDenom);
	Coefficient(int inpNonRootNumer, int inpDenom, int inpRootNumerator);
	Coefficient(int regularInteger);


	void printLatex() const;

	//For debugging
	void inputByUser();

	void simplify();

	Coefficient operator*(const Coefficient& rhs) const;
	Coefficient multiplyWithoutSimplifying(const Coefficient& rhs) const;

	Coefficient& operator*=(const Coefficient& rhs);
	Coefficient& multiplyAssignWithoutSimplifying(const Coefficient& rhs);

	//Assumes rootNumerator == 1
	Coefficient operator+(const Coefficient& rhs) const;
	Coefficient operator-(const Coefficient& rhs) const;
	Coefficient& operator=(const Coefficient& rhs);
	Coefficient& operator+=(const Coefficient& rhs);
	bool operator<(const Coefficient& rhs) const;
	bool operator>(const Coefficient& rhs) const;
	bool operator==(const Coefficient& rhs) const;
	bool operator!=(const Coefficient& rhs) const;
	bool operator<=(const Coefficient& rhs) const;
	bool operator>=(const Coefficient& rhs) const;


	int getNonRootNumerator() const {
		return nonRootNumerator;
	}
	int getRootNumerator() const {
		return rootNumerator;
	}
	int getDenominator() const {
		return denominator;
	}


private:
	void simplifyRoot();

	int nonRootNumerator;
	int rootNumerator;
	int denominator;
};

std::ostream& operator<<(std::ostream& os, const Coefficient& coefficient);



#endif
