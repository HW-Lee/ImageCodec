//
//  BitWriter.h
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Created by HW Lee on 4/13/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#ifndef BITWRITER
#define BITWRITER

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
class BitWriter {
public:
	static BitWriter* getInstance();
	static BitWriter* open(string path);
	void close();

	template <class T>
	void write(std::vector<T> data);
	template <class T>
	void write(T v, int size);

	void write(char v);
	void write(char* data, int size);

	string getPath();

private:
	BitWriter();
	static BitWriter* instance;
	static bool isInitialized;

	char byteBuffer;
	int currentBufferLength;

	string path;

	ofstream* os;
};

bool BitWriter::isInitialized = false;
BitWriter* BitWriter::instance = 0;

BitWriter* BitWriter::getInstance() {
	if (!isInitialized) { instance = new BitWriter(); isInitialized = true; }
	return instance;
}

BitWriter* BitWriter::open(string path) {
	remove(path.c_str());
	getInstance()->os = new ofstream(path.c_str(), ios::out|ios::binary|ios::app);
	getInstance()->path = path;
	return getInstance();
}

string BitWriter::getPath() { return this->path; }

template <class T>
void BitWriter::write(std::vector<T> data) {
	for (int i = 0; i < data.size(); i++) {
		this->byteBuffer = this->byteBuffer << 1;
		this->byteBuffer = this->byteBuffer + ( data[i] & 0x00000001 );
		this->currentBufferLength++;
		if (this->currentBufferLength == 8) {
			this->os->write(&this->byteBuffer, 1);
			this->currentBufferLength = 0;
			this->byteBuffer = 0;
		}
	}
}

template <class T>
void BitWriter::write(T v, int size) {
	long truncator = 1;
	std::vector<char> d;
	for (int i = 0; i < size; i++) d.push_back((v >> (size-1-i)) & truncator);
	this->write<char>(d);
}

void BitWriter::write(char v) { std::vector<char> d; d.push_back(v); this->write<char>(d); }
void BitWriter::write(char* data, int size) { this->os->write(data, size); }

void BitWriter::close() {
	if (this->currentBufferLength > 0) {
		this->byteBuffer = this->byteBuffer << (8-this->currentBufferLength);
		this->os->write(&this->byteBuffer, 1);
		this->byteBuffer = 0;
		this->currentBufferLength = 0;
	}
	this->os->close();
	this->path = "";
}

BitWriter::BitWriter() { this->byteBuffer = 0; this->currentBufferLength = 0; }

#endif
