#include "Coefficient.h"
#include "UtilityFunctions.h"
#include <cmath>

Coefficient::Coefficient(int inpNonRootNumer, int inpDenom) : nonRootNumerator(inpNonRootNumer), denominator(inpDenom), rootNumerator(1) { }
Coefficient::Coefficient(int regularInteger) : nonRootNumerator(regularInteger), denominator(1), rootNumerator(1) { }
Coefficient::Coefficient(int inpNonRootNumer, int inpDenom, int inpRootNumerator) : nonRootNumerator(inpNonRootNumer), denominator(inpDenom), rootNumerator(inpRootNumerator) { }





void Coefficient::inputByUser() {
	cout << "Numerator: ";
	cin >> nonRootNumerator;
	cout << "Denominator: ";
	cin >> denominator;
}

void Coefficient::simplify() {
	if (rootNumerator >= 4) {
		simplifyRoot();
	}

	int coeffGCD = gcd(nonRootNumerator, denominator);
	if (coeffGCD != 1) {
		nonRootNumerator /= coeffGCD;
		denominator /= coeffGCD;
	}
}

Coefficient Coefficient::operator*(const Coefficient& rhs) const {
	Coefficient result(nonRootNumerator*rhs.nonRootNumerator, denominator*rhs.denominator, rootNumerator*rhs.rootNumerator);
	result.simplify();
	return result;
}

Coefficient Coefficient::multiplyWithoutSimplifying(const Coefficient& rhs) const {
	Coefficient result(nonRootNumerator*rhs.nonRootNumerator, denominator*rhs.denominator, rootNumerator*rhs.rootNumerator);
	return result;
}


Coefficient Coefficient::operator+(const Coefficient& rhs) const {
	int denomLCM = lcm(denominator, rhs.denominator);
	Coefficient result((nonRootNumerator*(denomLCM/denominator)) + (rhs.nonRootNumerator*(denomLCM/rhs.denominator)), denomLCM);
	result.simplify();
	return result;
}

Coefficient Coefficient::operator-(const Coefficient& rhs) const {
	return ((*this) + (rhs*-1));
}

Coefficient& Coefficient::operator=(const Coefficient& rhs) {
	nonRootNumerator = rhs.nonRootNumerator;
	denominator = rhs.denominator;
	rootNumerator = rhs.rootNumerator;
	return *this;
}


Coefficient& Coefficient::operator*=(const Coefficient& rhs) {
	(*this) = operator*(rhs);
	return *this;
}

Coefficient& Coefficient::multiplyAssignWithoutSimplifying(const Coefficient& rhs) {
	(*this) = multiplyWithoutSimplifying(rhs);
	return *this;
}


Coefficient& Coefficient::operator+=(const Coefficient& rhs) {
	(*this) = operator+(rhs);
	return *this;
}


bool Coefficient::operator<(const Coefficient& rhs) const {
	//To take care of minute floating-point errors
	if (operator==(rhs)) {
		return true;
	}
	double lhsTrue = (1.0*nonRootNumerator) / denominator;
	double rhsTrue = (1.0*rhs.nonRootNumerator) / rhs.denominator;
	return (lhsTrue < rhsTrue);
}

bool Coefficient::operator>(const Coefficient& rhs) const {
	if (operator==(rhs)||operator<(rhs)) {
		return false;
	}
	return true;
}


bool Coefficient::operator==(const Coefficient& rhs) const {
	Coefficient lhsCopy(*this), rhsCopy(rhs);
	lhsCopy.simplify();
	rhsCopy.simplify();
	return ((lhsCopy.nonRootNumerator == rhsCopy.nonRootNumerator) && (lhsCopy.denominator == rhsCopy.denominator)&& (lhsCopy.rootNumerator == rhsCopy.rootNumerator));
}
bool Coefficient::operator!=(const Coefficient& rhs) const {
	return !(operator==(rhs));
}

bool Coefficient::operator<=(const Coefficient& rhs) const {
	if (operator==(rhs) || operator<(rhs)) {
		return true;
	}
	return false;
}

bool Coefficient::operator>=(const Coefficient& rhs) const {
	if (operator==(rhs) || operator>(rhs)) {
		return true;
	}
	return false;
}

void Coefficient::printLatex() const {
	if (nonRootNumerator > 0) {
		cout << "+";
	}
	else {
		cout << "-";
	}
	if (denominator == 1) {
		if (nonRootNumerator != 1) {
			cout << abs(nonRootNumerator);
		}
		if (rootNumerator != 1) {
			cout << "\\sqrt{" << rootNumerator << "}";
		}
	}
	else {
		cout << "\\frac{" << abs(nonRootNumerator);

		if (rootNumerator != 1) {
			cout << "\\sqrt{" << rootNumerator << "}";
		}
		cout << "}{" << denominator << "}";
	}
}

void Coefficient::simplifyRoot() {
	int j = 2;
	while (j < rootNumerator) {
		//If j squared divides it perfectly, do so, and move j outside the root. Reset j to search again
		if ((rootNumerator%(j*j)) == 0) {
			rootNumerator /= (j*j);
			nonRootNumerator *= j;
			j = 2;
		}
		else {
			++j;
		}
	}
}



//Non-member function definitions

std::ostream& operator<<(std::ostream& os, const Coefficient& coefficient) {
	os << coefficient.getNonRootNumerator();
	if (coefficient.getRootNumerator() != 1) {
		os << "V" << coefficient.getRootNumerator();
	}
	if (coefficient.getDenominator() != 1) {
		os << "/" << coefficient.getDenominator();
	}
	return os;
}





