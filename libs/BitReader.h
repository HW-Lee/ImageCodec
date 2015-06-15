//
//  BitReader.h
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Created by HW Lee on 4/13/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#ifndef BITREADER
#define BITREADER

#include <iostream>
#include <fstream>

using namespace std;
class BitReader {
public:
	static BitReader* getInstance();
	static BitReader* open(string path);
	streamsize getFileSize();
	streamsize remains();
	void close();
	char* read(int size);
	char read();

	template <class T>
	T read(int size);

private:
	BitReader();
	static BitReader* instance;
	static bool isInitialized;

	char* byteBuffer;
	int currentBufferLength;

	ifstream* is;
	streamsize size;
	streamsize _remains;
};

bool BitReader::isInitialized = false;
BitReader* BitReader::instance = 0;

BitReader* BitReader::getInstance() {
	if (!isInitialized) { instance = new BitReader(); isInitialized = true; }
	return instance;
}

BitReader* BitReader::open(string path) {
	getInstance()->is = new ifstream(path.c_str(), ios::in|ios::binary|ios::ate);
	if (getInstance()->is->is_open()) {
		getInstance()->size = getInstance()->is->tellg()*8;
		getInstance()->_remains = getInstance()->size;
		getInstance()->is->seekg(0, ios::beg);
	}
	else { getInstance()->size = -1; }
	return getInstance();
}

void BitReader::close() { getInstance()->is->close(); getInstance()->byteBuffer[0] = 0; getInstance()->currentBufferLength = 0; }

streamsize BitReader::getFileSize() { return this->instance->size; }
streamsize BitReader::remains() { return this->instance->_remains; }

char* BitReader::read(int size) {
	if (size < 1) return 0;
	char* outArr = new char[size];
	for (int i = 0; i < size; i++) {
		if (this->currentBufferLength == 0) { this->is->read(this->byteBuffer, 1); this->currentBufferLength = 8; }
		outArr[i] = ( (this->byteBuffer[0] >> 7) & 0x01 );
		this->byteBuffer[0] = this->byteBuffer[0] << 1;
		this->currentBufferLength--;
		this->instance->_remains--;
	}
	return outArr;
}

char BitReader::read() { char* out = this->read(1); return out[0]; }

template <class T>
T BitReader::read(int size) {
	T v = 0;
	for (int i = 0; i < size; i++) {
		v <<= 1;
		v |= (this->read() & 0x01);
	}
	return v;
}

BitReader::BitReader() { this->byteBuffer = 0; this->currentBufferLength = 0; this->byteBuffer = new char[1]; }

#endif
