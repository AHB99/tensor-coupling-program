#include "BbtChain.h"

void BbtChain::print() const {
	for (auto& bbt : bbtChain) {
		bbt.print(); 
		cout << " ";
	}
}

void BbtChain::printLatex() const {
	for (auto& bbt : bbtChain) {
		bbt.printLatex();
	}
}

void BbtChain::inputByUser() {
	cout << "Number of bbt: ";
	int tempNum;
	cin >> tempNum;
	for (int i = 0; i < tempNum; ++i) {
		Bbt tempBbt;
		tempBbt.inputByUser();
		bbtChain.push_back(tempBbt);
	}
}

Tensor BbtChain::convertToReducibleTensorEquivalent() const {
	Tensor resultReducibleTensor;
	resultReducibleTensor.setLabel("GAMMA");
	for (auto& bbt : bbtChain) {
		if (bbt.isDagger()) {
			resultReducibleTensor.addIndex(bbt.getName(), 1);
		}
		else {
			resultReducibleTensor.addIndex(bbt.getName(), 0);
		}
	}
	return resultReducibleTensor;
}

BbtChainCounter BbtChain::getBbtChainCounter() const {
	BbtChainCounter bbtChainCounter;
	for (int i = 0; i < bbtChain.size(); ++i) {
		if (isDaggerAt(i)) {
			++bbtChainCounter.btAfterBCount;
			++bbtChainCounter.btTotalCount;
		}
		//If b encountered, reset btAfterBcount because that must have been before bs.
		//Keep btTotalCount as is
		//Example form: bt bt b b b bt bt bt bt
		else {
			++bbtChainCounter.bCount;
			bbtChainCounter.btAfterBCount = 0;
		}
	}
	return bbtChainCounter;
}

void BbtChain::deleteBbtByRange(int first, int last) {
	bbtChain.erase(bbtChain.begin() + first, bbtChain.begin() + last);
}