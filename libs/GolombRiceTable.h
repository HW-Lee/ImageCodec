//
//  GolombRiceTable.h
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Created by HW Lee on 6/8/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#ifndef GOLOMBRICETABLE
#define GOLOMBRICETABLE

#include <iostream>
#include <math.h>
#include "Symbol.h"

using namespace std;

template <class T>
class GolombRiceTable {
public:
	~GolombRiceTable();
	static GolombRiceTable<T>* initWithSize(int size);
	GolombRiceTable<T>* withGroupSize(int gsize);
	GolombRiceTable<T>* withMethod(int opt);
	GolombRiceTable<T>* withExpRate(float rate);

	GolombRiceTable<T>* withData(T* data, int dataSize);

	GolombRiceTable<T>* init();

	Symbol<T>* getSymbol(int idx, int k);

	int size();
	int groupSize();
	int groupSize(int groupId);
	Symbol<T>* getSymbolAt(int idx);
	double getProbabilityAt(int idx);
	float bitrate();
	float entropy();

	void info();

	static const int CODING_DEFAULT_GR;
	static const int CODING_EXPONENTIAL_GR;

private:
	GolombRiceTable(int size);
	int getId(int symbIdx, int opt);

	int method;
	float r;
	Symbol<T>** symbols;
	double* p;
	int s;
	int gs;

	static const int ID_GROUP;
	static const int ID_ELEMENT;
	static const int ID_GROUP_START;

};

template <class T>
const int GolombRiceTable<T>::CODING_DEFAULT_GR = 0;
template <class T>
const int GolombRiceTable<T>::CODING_EXPONENTIAL_GR = 1;

template <class T>
const int GolombRiceTable<T>::ID_GROUP = 0;
template <class T>
const int GolombRiceTable<T>::ID_ELEMENT = 1;
template <class T>
const int GolombRiceTable<T>::ID_GROUP_START = 0;

template <class T>
GolombRiceTable<T>::~GolombRiceTable() { delete[] symbols; delete[] p; }
template <class T>
int GolombRiceTable<T>::size() { return this->s; }
template <class T>
int GolombRiceTable<T>::groupSize() { return this->groupSize(ID_GROUP_START); }
template <class T>
Symbol<T>* GolombRiceTable<T>::getSymbolAt(int idx) { return this->symbols[idx]; }
template <class T>
double GolombRiceTable<T>::getProbabilityAt(int idx) { return this->p[idx]; }

template <class T>
GolombRiceTable<T>* GolombRiceTable<T>::initWithSize(int size) { return new GolombRiceTable(size); }
template <class T>
GolombRiceTable<T>* GolombRiceTable<T>::withMethod(int method) { this->method = method; return this; }
template <class T>
GolombRiceTable<T>* GolombRiceTable<T>::withGroupSize(int gsize) { this->gs = gsize; return this; }
template <class T>
GolombRiceTable<T>* GolombRiceTable<T>::withExpRate(float rate) { this->r = rate; return this; }

template <class T>
int GolombRiceTable<T>::groupSize(int groupId) {
	int currentSize = this->s;
	float r = this->r;
	int size = this->gs;
	for (int i = ID_GROUP_START; i < groupId; i++) { currentSize -= size; if (this->method == CODING_EXPONENTIAL_GR) size = (int)round(r * size); }
	currentSize = max(0, currentSize);
	return min(size, currentSize);
}

template <class T>
GolombRiceTable<T>* GolombRiceTable<T>::withData(T* data, int dataSize) {
	long truncator = 0;
	truncator = ~truncator;
	for (int i = 0; i < sizeof(long); i++) truncator <<= 8;
	truncator = ~truncator;

	this->p = new double[this->s];
	for (int i = 0; i < this->s; i++) this->p[i] = 0;
	for (int i = 0; i < dataSize; i++) { this->p[ (long)(data[i] & truncator) ]++; }
	for (int i = 0; i < this->s; i++) this->p[i] /= dataSize;
	return this;
}

template <class T>
Symbol<T>* GolombRiceTable<T>::getSymbol(int idx, int k) {
	int m = 1;
	for (int i = 0; i < k; i++) m *= 2;
	this->gs = m; this->method = CODING_DEFAULT_GR;

	Symbol<T>* symbol = new Symbol<T>(idx);
	int id = this->getId(idx, ID_GROUP);
	int suffixV = this->getId(idx, ID_ELEMENT);
	int currentSize = this->groupSize(id);
	int b = ceil(log(currentSize)/log(2));


	if (suffixV < pow(2, b) - currentSize) b--;
	else suffixV += ( pow(2, b) - currentSize );
	for (int i = 0; i < b; i++) { symbol->pushWord(suffixV % 2); suffixV >>= 1; }
	symbol->pushWord_0();
	for (int i = 0; i < id; i++) symbol->pushWord_1();
	return symbol;
}

template <class T>
GolombRiceTable<T>* GolombRiceTable<T>::init() {
	if (this->s <= 0 || this->gs <= 0) { cout << "warning: the size/groupSize is incorrect, the process has been aborted." << endl; return this; }
	this->symbols = new Symbol<T>*[this->s];
	for (int i = 0; i < this->s; i++) { 
		this->symbols[i] = new Symbol<T>(i);

		int id = this->getId(i, ID_GROUP);
		int suffixV = this->getId(i, ID_ELEMENT);
		int currentSize = this->groupSize(id);
		int b = ceil(log(currentSize)/log(2));

		// Suffix
		if (suffixV < pow(2, b) - currentSize) b--;
		else suffixV += ( pow(2, b) - currentSize );
		for (int j = 0; j < b; j++) { this->symbols[i]->pushWord(suffixV % 2); suffixV >>= 1; }

		// Group Id
		this->symbols[i]->pushWord_0();
		for (int j = 0; j < id; j++) this->symbols[i]->pushWord_1();
	}
	return this;
}

template <class T>
int GolombRiceTable<T>::getId(int symbIdx, int opt) {
	int id = ID_GROUP_START, currentSize = this->gs, curIdx = symbIdx;
	float rate = 1;
	while (1) {
		if (this->method == CODING_EXPONENTIAL_GR) { currentSize = (int)round(rate * this->gs); rate *= this->r; }
		if (currentSize > curIdx) { return (opt == ID_GROUP) ? id : curIdx; }
		else { curIdx -= currentSize; id++; }
	}
}

template <class T>
float GolombRiceTable<T>::bitrate() {
	float br = 0;
	for (int i = 0; i < this->size(); i++)
		br += this->getProbabilityAt(i)*this->getSymbolAt(i)->getWordSize();
	return br;
}

template <class T>
float GolombRiceTable<T>::entropy() {
	float e = 0;
	for (int i = 0; i < this->size(); i++)
		if (this->getProbabilityAt(i) > 0) e += -(this->getProbabilityAt(i)*log(this->getProbabilityAt(i))/log(2));
	return e;
}

template <class T>
void GolombRiceTable<T>::info() {
	string methodStr = (this->method == CODING_DEFAULT_GR) ? "Fixed-size-GR" : "Exp-GR";
	cout << "Coding method: " << methodStr << endl;
	cout << "# of symbols: " << this->size() << endl;
	cout << "initial group size: " << this->groupSize() << endl;
	for (int i = 0; i < this->s; i++) {
		if (this->p[i] == 0) continue;
		cout << "symbols[" << i << "] :";
		cout << (int)(this->symbols[i]->getValue() & 0xffffffff) << ", ";
		cout << "p[" << i << "]: " << this->p[i] << ", ";
		cout << "w[" << i << "]: \"" << this->symbols[i]->getWordString() << "\"";
		cout << ", v = " << this->symbols[i]->template getWordValue<long>();
		cout << endl;
	}
	cout << "Entropy: " << this->entropy() << endl;
	cout << "Bit rate: " << this->bitrate() << endl;
}

template <class T>
GolombRiceTable<T>::GolombRiceTable(int size) { this->s = size; this->gs = -1; this->method = CODING_DEFAULT_GR; this->r = 2; }

#endif
