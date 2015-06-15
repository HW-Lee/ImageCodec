//
//  HuffmanTable.h
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Created by HW Lee on 4/13/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#ifndef HUFFMANTABLE
#define HUFFMANTABLE

#include <iostream>
#include <math.h>
#include <iomanip>
#include "Symbol.h"

using namespace std;

template <class T>
class HuffmanTable {
public:
	~HuffmanTable();
	static HuffmanTable<T>* initWithSize(int size);

	HuffmanTable<T>* withData(T* data, int dataSize, T minSymbol);
	HuffmanTable<T>* withData(T* data, int dataSize);

	int size();
	Symbol<T>* getSymbolAt(int idx);
	double getProbabilityAt(int idx);
	int getMaxWordLength();
	int getNumEntriesWithWordLength(int wordlength);
	int getMethod();
	float bitrate();
	float entropy();

	static HuffmanTable<T>* initWithMaxWordLength(int wl);
	HuffmanTable<T>* withNumEntries(int* nEntries);

	void info();

	static const int CODING_DIRECT;
	static const int CODING_DIFF;

private:
	HuffmanTable(int size);
	HuffmanTable(int size, int wl);
	int method;
	Symbol<T>** symbols;
	double* p;
	int s;

	int maxWordLength;
	int* nEntries;
};

template <class T>
HuffmanTable<T>::~HuffmanTable() { delete[] symbols; delete[] p; delete[] nEntries; }

template <class T>
const int HuffmanTable<T>::CODING_DIRECT = 0;
template <class T>
const int HuffmanTable<T>::CODING_DIFF = 1;

template <class T>
int HuffmanTable<T>::size() { return this->s; }
template <class T>
Symbol<T>* HuffmanTable<T>::getSymbolAt(int idx) { return this->symbols[idx]; }
template <class T>
double HuffmanTable<T>::getProbabilityAt(int idx) { return this->p[idx]; }
template <class T>
int HuffmanTable<T>::getMaxWordLength() { return this->maxWordLength; }
template <class T>
int HuffmanTable<T>::getNumEntriesWithWordLength(int wordlength) {
	return (wordlength > this->maxWordLength || wordlength < 1) ? 0 : this->nEntries[wordlength-1];
}

template <class T>
HuffmanTable<T>* HuffmanTable<T>::initWithSize(int size) { return new HuffmanTable(size); }

template <class T>
HuffmanTable<T>* HuffmanTable<T>::initWithMaxWordLength(int wl) { return new HuffmanTable(0, wl); }
template <class T>
HuffmanTable<T>* HuffmanTable<T>::withNumEntries(int* nEntries) { for (int i = 0; i < this->maxWordLength; i++) this->nEntries[i] = nEntries[i]; return this; }

template <class T>
HuffmanTable<T>* HuffmanTable<T>::withData(T* data, int dataSize) { return this->withData(data, dataSize, 0); }

template <class T>
HuffmanTable<T>* HuffmanTable<T>::withData(T* data, int dataSize, T minSymbol) {
	long truncator = 0;
	truncator = ~truncator;
	for (int i = 0; i < sizeof(T); i++) truncator <<= 8;
	truncator = ~truncator;

	int nSymbol = this->s;
	if (this->method == CODING_DIRECT) {
		this->s = nSymbol;
		this->symbols = new Symbol<T>*[this->s];
		this->p = new double[this->s];

		// init probabilities
		for (int i = 0; i < this->s; i++) this->p[i] = 0;

		// compute the frequencies of occurrence of each symbol
		for (int i = 0; i < dataSize; i++) this->p[ data[i] - minSymbol ]++;

		// symbols[0] = 0, symbols[end] = nSymbol
		for (int i = 0; i < this->s; i++) this->symbols[i] = new Symbol<T>(i+minSymbol);

	}

	// normalize the occurrence as a measure of probability
	for (int i = 0; i < this->s; i++) {
		if (this->p[i] == 0) {
			for (int j = this->s-1; j > i; j--) {
				if (this->p[j] > 0) {
					double p = this->p[i]; this->p[i] = this->p[j]; this->p[j] = p;
					Symbol<T>* symbol = this->symbols[i]; this->symbols[i] = this->symbols[j]; this->symbols[j] = symbol;
					this->s--;
					break;
				}
			}
		} 
		this->p[i] /= (double)dataSize;
	}

	// assign the codeword of each symbol based on the probability it has
	int lastIdx = this->s - 1;
	std::vector<T>* nodes = new std::vector<T>[this->s];
	double* cur_p = new double[this->s];

	for (int i = 0; i < this->s; i++) {
		std::vector<T> v;
		v.push_back(i);
		nodes[i] = v;
		cur_p[i] = this->p[i];
	}

	while (lastIdx > 0) {
		for (int i = lastIdx; i >= lastIdx-1; i--) {
			// find the symbol which has the 2 lowest probability of occurrence
			for (int j = i; j >= 0; j--) {
				if (cur_p[i] > cur_p[j]) {
					double p = cur_p[i]; cur_p[i] = cur_p[j]; cur_p[j] = p;
					std::vector<T> v = nodes[i]; nodes[i] = nodes[j]; nodes[j] = v;
				}
			}
		}
		if (cur_p[lastIdx] > 0) {
			std::vector<T> v;
			for (int i = 0; i < nodes[lastIdx].size(); i++) {
				v.push_back(nodes[lastIdx][i]);
				this->symbols[ nodes[lastIdx][i] ]->pushWord_0();
			}
			for (int i = 0; i < nodes[lastIdx-1].size(); i++) {
				v.push_back(nodes[lastIdx-1][i]);
				this->symbols[ nodes[lastIdx-1][i] ]->pushWord_1();
			}
			nodes[lastIdx-1] = v;
			cur_p[lastIdx-1] += cur_p[lastIdx];
			nodes[lastIdx].clear();
		}
		lastIdx--;
	}

	// sort symbols with its word length
	for (int i = 0; i < this->s-1; i++) {
		for (int j = i+1; j < this->s; j++) {
			if (this->symbols[i]->getWordSize() > this->symbols[j]->getWordSize()) {
				Symbol<T>* temp = this->symbols[i];
				this->symbols[i] = this->symbols[j];
				this->symbols[j] = temp;
				double p = this->p[i];
				this->p[i] = this->p[j];
				this->p[j] = p;
			}
		}
	}

	// eliminate redundant symbols, which have zero length words
	int nonZeroProbIdx = 0;
	for (nonZeroProbIdx = 0; nonZeroProbIdx < this->s; nonZeroProbIdx++) if (this->p[nonZeroProbIdx] > 0) break;
	for (int i = 0; i < this->s - nonZeroProbIdx; i++) {
		this->symbols[i] = this->symbols[i+nonZeroProbIdx];
		this->p[i] = this->p[i+nonZeroProbIdx];
	}
	this->s = this->s - nonZeroProbIdx;

	// reassign codewords following the fomation of canonical huffman coding
	this->maxWordLength = this->getSymbolAt(this->s-1)->getWordSize();
	this->nEntries = new int[this->maxWordLength];

	for (int i = 0; i < this->maxWordLength; i++) this->nEntries[i] = 0;
	for (int i = 0; i < this->s; i++) this->nEntries[ this->symbols[i]->getWordSize()-1 ]++;

	int curL = this->symbols[0]->getWordSize();
	int curR = 0;

	this->symbols[0]->clearWord();
	for (int i = 0; i < curL; i++) this->symbols[0]->pushWord_0();

	for (int i = 1; i < this->s; i++) {
		if (this->symbols[i]->getWordSize() > curL) { curR++; curR <<= (this->symbols[i]->getWordSize()-curL) ; curL = this->symbols[i]->getWordSize(); }
		else { curR++; }
		int v = curR;
		this->symbols[i]->clearWord();
		for (int j = 0; j < curL; j++) {
			if (v % 2 == 0) this->symbols[i]->pushWord_0();
			else this->symbols[i]->pushWord_1();
			v = v >> 1;
		}
	}

	return this;
}

template <class T>
float HuffmanTable<T>::bitrate() {
	float br = 0;
	for (int i = 0; i < this->size(); i++)
		br += this->getProbabilityAt(i)*this->getSymbolAt(i)->getWordSize();
	return br;
}

template <class T>
float HuffmanTable<T>::entropy() {
	float e = 0;
	for (int i = 0; i < this->size(); i++)
		if (this->getProbabilityAt(i) > 0) e += -(this->getProbabilityAt(i)*log(this->getProbabilityAt(i))/log(2));
	return e;
}

template <class T>
HuffmanTable<T>::HuffmanTable(int size) {
	this->s = size;
	this->maxWordLength = 0;
	this->nEntries = 0;
	this->method = CODING_DIRECT;
}

template <class T>
HuffmanTable<T>::HuffmanTable(int size, int wl) {
	this->s = size;
	this->maxWordLength = wl;
	this->nEntries = new int[wl];
	this->method = CODING_DIRECT;
}

template <class T>
void HuffmanTable<T>::info() {
	string methodStr = (this->method == CODING_DIRECT) ? "direct" : "simple differential";
	cout << "Coding method: " << methodStr << endl;
	cout << "# of Symbols: " << this->size() << endl;
	for (int i = 0; i < this->s; i++) {
		cout << "symbol[" << i << "]: ";
		cout << (int)(this->symbols[i]->getValue() & 0xffffffff) << ", ";
		cout << "p[" << i << "]: " << this->p[i] << ", ";
		cout << "w[" << i << "]: \"" << this->symbols[i]->getWordString() << "\"";
		cout << ", v = " << this->symbols[i]->template getWordValue<long>();
		cout << endl;
	}
	cout << "Canonical word length v.s. # entries:" << endl;
	for (int i = 0; i < this->maxWordLength; i++) {
		cout << "length = " << (i+1) << ", " << this->nEntries[i] << " entries.";
		cout << endl;
	}
	cout << "Entropy: " << this->entropy() << endl;
	cout << "Bit rate: " << this->bitrate() << endl;

	// generating the table following the formation of LaTeX syntax
	// cout << "\\hline" << endl;
	// for (int i = 0; i < this->s/2 + this->s%2; i++) {
	// 	cout << i << " & " << (int)this->symbols[i]->getValue() << " & $";
	// 	cout << std::setprecision(3) << std::scientific << this->p[i] << "$ & ";
	// 	cout << "\"" << this->symbols[i]->getWordString() << "\"";
	// 	cout << " & " << this->symbols[i]->getWordValue() << " & ";
	// 	if (i + this->s/2 + this->s%2 < this->s) {
	// 		cout << (i + this->s/2 + this->s%2) << " & " << (int)this->symbols[i + this->s/2 + this->s%2]->getValue() << " & $";
	// 		cout << std::setprecision(3) << std::scientific << this->p[i + this->s/2 + this->s%2] << "$ & ";
	// 		cout << "\"" << this->symbols[i + this->s/2 + this->s%2]->getWordString() << "\"";
	// 		cout << " & " << this->symbols[i + this->s/2 + this->s%2]->getWordValue();
	// 	}
	// 	cout << " \\\\";
	// 	cout << endl;
	// 	cout << "\\hline" << endl;
	// }

	// cout << "\\hline" << endl;
	// cout << "Canonical word length v.s. # entries:" << endl;
	// for (int i = 0; i < this->maxWordLength; i++) {
	// 	cout << (i+1) << " & " << this->nEntries[i] << " \\\\";
	// 	cout << endl;
	// 	cout << "\\hline" << endl;
	// }
}

#endif
