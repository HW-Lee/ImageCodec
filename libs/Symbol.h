//
//  Symbol.h
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Created by HW Lee on 4/13/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#ifndef SYMBOL
#define SYMBOL

#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

template <class T>
class Symbol {
public:
	Symbol(T v);
	~Symbol();

	T getValue();
	std::vector<T> getWord();
	int getWordSize();
	string getWordString();

	template <class VT>
	VT getWordValue();
	
	void pushWord(char v);
	void pushWord_1();
	void pushWord_0();
	void clearWord();

private:
	T v;
	std::vector<T> word;
};

template <class T>
Symbol<T>::Symbol(T v) { this->v = v; std::vector<T> w(0); this->word = w; }
template <class T>
T Symbol<T>::getValue() { return this->v; }
template <class T>
std::vector<T> Symbol<T>::getWord() { return this->word; }
template <class T>
int Symbol<T>::getWordSize() { return this->word.size(); }
template <class T>
void Symbol<T>::pushWord(char v) { this->word.insert(this->word.begin(), v % 2); }
template <class T>
void Symbol<T>::pushWord_0() { this->word.insert(this->word.begin(), 0); }
template <class T>
void Symbol<T>::pushWord_1() { this->word.insert(this->word.begin(), 1); }
template <class T>
void Symbol<T>::clearWord() { this->word.clear(); }

template <class T>
string Symbol<T>::getWordString() {
	stringstream ss;
	if (this->word.size() == 0) return "";
	for (int i = 0; i < this->word.size(); i++) ss << (int)(this->word[i]); // Big Endian
	return ss.str();
}

template <class T> template <class VT>
VT Symbol<T>::getWordValue() {
	VT v = 0;
	for (int i = 0; i < this->word.size(); i++) v = v*2 + this->word[i];
	return v;
}

template <class T>
Symbol<T>::~Symbol() { this->word.clear(); }

#endif
