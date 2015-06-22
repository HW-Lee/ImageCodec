//
//  YUVImage.h
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Created by HW Lee on 6/2/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#ifndef YUVIMAGE
#define YUVIMAGE

#include <iostream>
#include <fstream>
#include <math.h>
#include "Transform.h"

using namespace std;

class YUVImage {
public:
	~YUVImage();
	static YUVImage* import(string path);
	static YUVImage* emptyImage(string path, int width, int height, int format);
	YUVImage* withSize(int width, int height);
	YUVImage* withFormat(int format);

	YUVImage* transformToFormat(int format);

	YUVImage* diffReordering();
	YUVImage* inv_diffReordering();
	YUVImage* diffReordering(int Y_max, int U_max, int V_max);
	YUVImage* inv_diffReordering(int Y_max, int U_max, int V_max);

	YUVImage* diff(YUVImage* image);
	YUVImage* add(YUVImage* image);
	YUVImage* decorrelate();

	int getWidth(int opt);
	int getHeight(int opt);
	short getMax(int opt);
	short getMin(int opt);
	void info();
	void exportTo(string path);

	string getPath();
	string getName();

	template <class T>
	T getYDataAt(int x, int y);
	template <class T>
	T getYDataAt(int idx);
	void setYDataAt(int x, int y, short d);
	void setYDataAt(int idx, short d);

	template <class T>
	T getCbDataAt(int x, int y);
	template <class T>
	T getCbDataAt(int idx);
	void setCbDataAt(int x, int y, short d);
	void setCbDataAt(int idx, short d);

	template <class T>
	T getCrDataAt(int x, int y);
	template <class T>
	T getCrDataAt(int idx);
	void setCrDataAt(int x, int y, short d);
	void setCrDataAt(int idx, short d);

	template <class T>
	T getDataAt(int idx);
	void setDataAt(int idx, short d);

	int getDataSize(int opt);
	int getDataSize();
	int getFormat();

	double calPSNR(YUVImage* image);
	double calBitrate(string path);

	YUVImage* clone();
	YUVImage* downSample(int wscale, int hscale);
	YUVImage* downSample(int scale);
	YUVImage* upSample(int wscale, int hscale);
	YUVImage* upSample(int scale);

	static const int FORMAT_4_4_4;
	static const int FORMAT_4_2_2;
	static const int FORMAT_4_2_0;
	static const string FORMAT_4_4_4_STR;
	static const string FORMAT_4_2_2_STR;
	static const string FORMAT_4_2_0_STR;

private:
	YUVImage(string path);
	void startImport();
	bool isSetup();
	void setSize(int width, int height);
	void init();
	int getOffset(int opt);

	bool replace(std::string& str, const std::string& from, const std::string& to);

	float getScale();

	string imgPath;
	int imgWidth;
	int imgHeight;
	int imgFormat;
	short* data;
	int dataSize;

	static const int FORMAT_UNDEFINED;

};

class DataLayer { public: const static int Y, Cb, Cr; };
const int DataLayer::Y = 0;
const int DataLayer::Cb = 1;
const int DataLayer::Cr = 2;

const int YUVImage::FORMAT_UNDEFINED = -1;
const int YUVImage::FORMAT_4_4_4 = 0;
const int YUVImage::FORMAT_4_2_2 = 1;
const int YUVImage::FORMAT_4_2_0 = 2;
const string YUVImage::FORMAT_4_4_4_STR = "4:4:4";
const string YUVImage::FORMAT_4_2_2_STR = "4:2:2";
const string YUVImage::FORMAT_4_2_0_STR = "4:2:0";

YUVImage* YUVImage::clone() {
	YUVImage* img = new YUVImage(this->imgPath);
	img->setSize(this->imgWidth, this->imgHeight);
	img->imgFormat = this->imgFormat;
	img->dataSize = this->dataSize;
	img->data = new short[img->dataSize];
	for (int i = 0; i < img->dataSize; i++) img->data[i] = this->getDataAt<short>(i);
	return img;
}

YUVImage* YUVImage::upSample(int wscale, int hscale) {
	if (wscale == 1 && hscale == 1) { return this->clone(); }
	YUVImage* img = emptyImage(this->getPath(), imgWidth*wscale, imgHeight*hscale, this->getFormat());
	for (int x = 0; x < this->getWidth(DataLayer::Y); x++) {
		for (int y = 0; y < this->getHeight(DataLayer::Y); y++) {
			for (int xs = 0; xs < wscale; xs++)
				for (int ys = 0; ys < hscale; ys++)
					img->setYDataAt(x*wscale+xs, y*hscale+ys, this->getYDataAt<short>(x, y));
		}
	}
	for (int x = 0; x < this->getWidth(DataLayer::Cb); x++) {
		for (int y = 0; y < this->getHeight(DataLayer::Cb); y++) {
			for (int xs = 0; xs < wscale; xs++)
				for (int ys = 0; ys < hscale; ys++)
					img->setCbDataAt(x*wscale+xs, y*hscale+ys, this->getCbDataAt<short>(x, y));
		}
	}
	for (int x = 0; x < this->getWidth(DataLayer::Cr); x++) {
		for (int y = 0; y < this->getHeight(DataLayer::Cr); y++) {
			for (int xs = 0; xs < wscale; xs++)
				for (int ys = 0; ys < hscale; ys++)
					img->setCrDataAt(x*wscale+xs, y*hscale+ys, this->getCrDataAt<short>(x, y));
		}
	}
	return img;
}

YUVImage* YUVImage::downSample(int wscale, int hscale) {
	if (wscale == 1 && hscale == 1) { return this->clone(); }
	YUVImage* img = emptyImage(this->getPath(), imgWidth/wscale, imgHeight/hscale, this->getFormat());
	short* v = new short[wscale*hscale];
	for (int x = 0; x < img->getWidth(DataLayer::Y); x++) {
		for (int y = 0; y < img->getHeight(DataLayer::Y); y++) {
			for (int xs = 0; xs < wscale; xs++)
				for (int ys = 0; ys < hscale; ys++)
					v[ys*wscale+xs] = this->getYDataAt<short>(x*wscale+xs, y*hscale+ys);
			for (int i = 0; i < wscale*hscale-1; i++)
				for (int j = i+1; j < wscale*hscale; j++)
					if (v[i] > v[j]) { short temp = v[i]; v[i] = v[j]; v[j] = temp; }
			img->setYDataAt(x, y, v[wscale*hscale/2]);
		}
	}
	for (int x = 0; x < img->getWidth(DataLayer::Cb); x++) {
		for (int y = 0; y < img->getHeight(DataLayer::Cb); y++) {
			for (int xs = 0; xs < wscale; xs++)
				for (int ys = 0; ys < hscale; ys++)
					v[ys*wscale+xs] = this->getCbDataAt<short>(x*wscale+xs, y*hscale+ys);
			for (int i = 0; i < wscale*hscale-1; i++)
				for (int j = i+1; j < wscale*hscale; j++)
					if (v[i] > v[j]) { short temp = v[i]; v[i] = v[j]; v[j] = temp; }
			img->setCbDataAt(x, y, v[wscale*hscale/2]);
		}
	}
	for (int x = 0; x < img->getWidth(DataLayer::Cr); x++) {
		for (int y = 0; y < img->getHeight(DataLayer::Cr); y++) {
			for (int xs = 0; xs < wscale; xs++)
				for (int ys = 0; ys < hscale; ys++)
					v[ys*wscale+xs] = this->getCrDataAt<short>(x*wscale+xs, y*hscale+ys);
			for (int i = 0; i < wscale*hscale-1; i++)
				for (int j = i+1; j < wscale*hscale; j++)
					if (v[i] > v[j]) { short temp = v[i]; v[i] = v[j]; v[j] = temp; }
			img->setCrDataAt(x, y, v[wscale*hscale/2]);
		}
	}
	return img;
}

YUVImage* YUVImage::downSample(int scale) { return this->downSample(scale, scale); }
YUVImage* YUVImage::upSample(int scale) { return this->upSample(scale, scale); }

YUVImage* YUVImage::import(string path) { return new YUVImage(path); }
YUVImage* YUVImage::withSize(int width, int height) { this->setSize(width, height); if (this->isSetup()) this->startImport(); return this; }
YUVImage* YUVImage::withFormat(int format) { this->imgFormat = format; if (this->isSetup()) this->startImport(); return this; }

string YUVImage::getPath() { return this->imgPath; }

string YUVImage::getName() {
	string p = this->getPath();
	this->replace(p, "./", "");
	this->replace(p, "/", "");
	return p;
}

YUVImage* YUVImage::emptyImage(string path, int width, int height, int format) {
	YUVImage* image = new YUVImage(path);
	image->setSize(width, height);
	image->imgFormat = format;
	image->init();
	for (int i = 0; i < image->dataSize; i++) image->data[i] = 0;
	return image;
}

YUVImage* YUVImage::transformToFormat(int format) {
	YUVImage* image = this->clone();
	if (this->imgFormat == format) { delete image; return this; }
	bool isUpSampling = (this->imgFormat - format) > 0;
	this->imgFormat = format;
	delete[] this->data; this->init();

	int wScale, hScale;
	for (int i = 0; i < this->getDataSize(DataLayer::Y); i++) { this->setYDataAt(i, image->getYDataAt<short>(i)); }
	for (int opt = DataLayer::Cb; opt <= DataLayer::Cr; opt++) {
		if (isUpSampling) {
			wScale = this->getWidth(DataLayer::Cb)/image->getWidth(opt);
			hScale = this->getHeight(DataLayer::Cb)/image->getHeight(opt);
		} else {
			wScale = image->getWidth(opt)/this->getWidth(DataLayer::Cb);
			hScale = image->getHeight(opt)/this->getHeight(DataLayer::Cb);
		}
		for (int x = 0; x < this->getWidth(opt); x++)
			for (int y = 0; y < this->getHeight(opt); y++)
				if (opt == DataLayer::Cb && isUpSampling) this->setCbDataAt(x, y, image->getCbDataAt<short>(x/wScale, y/hScale));
				else if (opt == DataLayer::Cr && isUpSampling) this->setCrDataAt(x, y, image->getCrDataAt<short>(x/wScale, y/hScale));
				else if (opt == DataLayer::Cb && !isUpSampling) {
					short v = 0;
					for (int i = 0; i < hScale; i++) v += image->getCbDataAt<short>(x*wScale, y*hScale + i);
					v /= hScale; this->setCbDataAt(x, y, v); v = 0;
				} else {
					short v = 0;
					for (int i = 0; i < hScale; i++) v += image->getCrDataAt<short>(x*wScale, y*hScale + i);
					v /= hScale; this->setCrDataAt(x, y, v); v = 0;
				}
	}

	delete image;
	return this;
}

YUVImage* YUVImage::diffReordering() { return this->diffReordering(256, 256, 256); }
YUVImage* YUVImage::inv_diffReordering() { return this->inv_diffReordering(256, 256, 256); }

YUVImage* YUVImage::diffReordering(int Y_max, int U_max, int V_max) {
	for (int i = 0; i < this->getDataSize(DataLayer::Y); i++)
		if (this->getYDataAt<short>(i) < Y_max/2) this->setYDataAt(i, 2*this->getYDataAt<short>(i));
		else this->setYDataAt(i, 2 * (Y_max - this->getYDataAt<short>(i)) - 1);
	for (int i = 0; i < this->getDataSize(DataLayer::Cb); i++)
		if (this->getCbDataAt<short>(i) < U_max/2) this->setCbDataAt(i, 2*this->getCbDataAt<short>(i));
		else this->setCbDataAt(i, 2 * (U_max - this->getCbDataAt<short>(i)) - 1);
	for (int i = 0; i < this->getDataSize(DataLayer::Cr); i++)
		if (this->getCrDataAt<short>(i) < V_max/2) this->setCrDataAt(i, 2*this->getCrDataAt<short>(i));
		else this->setCrDataAt(i, 2 * (V_max - this->getCrDataAt<short>(i)) - 1);
	return this;
}

YUVImage* YUVImage::inv_diffReordering(int Y_max, int U_max, int V_max) {
	for (int i = 0; i < this->getDataSize(DataLayer::Y); i++)
		if (this->getYDataAt<short>(i) % 2 == 0) this->setYDataAt(i, this->getYDataAt<short>(i)/2);
		else this->setYDataAt(i, Y_max - (this->getYDataAt<short>(i) + 1)/2);
	for (int i = 0; i < this->getDataSize(DataLayer::Cb); i++)
		if (this->getCbDataAt<short>(i) % 2 == 0) this->setCbDataAt(i, this->getCbDataAt<short>(i)/2);
		else this->setCbDataAt(i, U_max - (this->getCbDataAt<short>(i) + 1)/2);
	for (int i = 0; i < this->getDataSize(DataLayer::Cr); i++)
		if (this->getCrDataAt<short>(i) % 2 == 0) this->setCrDataAt(i, this->getCrDataAt<short>(i)/2);
		else this->setCrDataAt(i, V_max - (this->getCrDataAt<short>(i) + 1)/2);
	return this;
}

YUVImage* YUVImage::diff(YUVImage* image) {
	YUVImage* diffImg = this->clone();
	if ( diffImg->getDataSize(DataLayer::Y) != image->getDataSize(DataLayer::Y) 
		|| diffImg->getDataSize(DataLayer::Cb) != image->getDataSize(DataLayer::Cb)
		|| diffImg->getDataSize(DataLayer::Cr) != image->getDataSize(DataLayer::Cr) ) { 
		cout << "the size mismatched in YUVImage::diff()" << endl; return diffImg; }
	for (int i = 0; i < diffImg->dataSize; i++) { diffImg->data[i] = ( this->data[i] - image->data[i] + 256 ) % 256; }
	return diffImg;
}

YUVImage* YUVImage::add(YUVImage* image) {
	YUVImage* addedImg = this->clone();
	if ( addedImg->getDataSize(DataLayer::Y) != image->getDataSize(DataLayer::Y) 
		|| addedImg->getDataSize(DataLayer::Cb) != image->getDataSize(DataLayer::Cb)
		|| addedImg->getDataSize(DataLayer::Cr) != image->getDataSize(DataLayer::Cr) ) { 
		cout << "the size mismatched in YUVImage::add()" << endl; return addedImg; }
	for (int i = 0; i < addedImg->dataSize; i++) { addedImg->data[i] = ( this->data[i] + image->data[i] ) % 256; }
	return addedImg;
}

YUVImage* YUVImage::decorrelate() {
	YUVImage* img = this->clone();
	Transform::dct4<short>(img->data, img->dataSize);
	return img;
}

void YUVImage::exportTo(string path) {
	remove(path.c_str());
	ofstream file(path.c_str(), ios::out|ios::app|ios::binary);
	for (int i = 0; i < this->dataSize; i++) { char v = (char)(this->data[i] & 0x00ff); file.write(&v, 1); }
	file.close();
}

void YUVImage::startImport() {
	this->init();
	ifstream file(this->imgPath.c_str(), ios::in|ios::binary|ios::ate);
	streamsize size;
	char* rawData;
	if (file.is_open()) {
		size = file.tellg();
		rawData = new char[size];
		file.seekg(0, ios::beg);
		file.read(rawData, size);
		file.close();
		if (size >= this->dataSize && this->dataSize > 0) {
			for (int i = 0; i < this->dataSize; i++) this->data[i] = (short)(rawData[i] & 0x00ff);
			delete[] rawData;
		} else {
			cout << "Import error: the format is wrong or the data is broken!" << endl;
			delete[] this->data;
		}
	}
}

double YUVImage::calBitrate(string path) {
	ifstream file(path.c_str(), ios::in|ios::binary|ios::ate);
	streamsize size;
	if (file.is_open()) {
		size = file.tellg();
		return 8.0*size / this->getDataSize(DataLayer::Y);
	}
	return -1;
}

void YUVImage::init() {
	float scale = this->getScale();
	this->dataSize = (int)(this->imgWidth * this->imgHeight * scale);
	this->data = new short[this->dataSize];
}

float YUVImage::getScale() {
	if (this->imgFormat == FORMAT_4_4_4) return 3;
	else if (this->imgFormat == FORMAT_4_2_2) return 2;
	else if (this->imgFormat == FORMAT_4_2_0) return 1.5;
	return 0;
}

void YUVImage::info() {
	cout << "path: \"" << this->imgPath << "\"" << endl;
	string formatStr;
	if (this->imgFormat == FORMAT_4_4_4) formatStr = FORMAT_4_4_4_STR;
	else if (this->imgFormat == FORMAT_4_2_2) formatStr = FORMAT_4_2_2_STR;
	else if (this->imgFormat == FORMAT_4_2_0) formatStr = FORMAT_4_2_0_STR;
	else formatStr = "";
	cout << "format: " << formatStr << endl;
	cout << "width: " << this->imgWidth << ", height: " << this->imgHeight << endl; 
	cout << "dataSize: " << this->dataSize << endl;
}

bool YUVImage::isSetup() { return (this->imgWidth > 0 && this->imgHeight > 0 && this->imgFormat >= 0); }
void YUVImage::setSize(int width, int height) { this->imgWidth = width; this->imgHeight = height; }

template <class T>
T YUVImage::getDataAt(int idx) { return (T)(this->data[idx] & 0x0000ffff); }
template <class T>
T YUVImage::getYDataAt(int idx) { return this->getDataAt<T>(this->getOffset(DataLayer::Y) + idx); }
template <class T>
T YUVImage::getCbDataAt(int idx) { return this->getDataAt<T>(this->getOffset(DataLayer::Cb) + idx); }
template <class T>
T YUVImage::getCrDataAt(int idx) { return this->getDataAt<T>(this->getOffset(DataLayer::Cr) + idx); }
template <class T>
T YUVImage::getYDataAt(int x, int y) { return this->getYDataAt<T>(y*this->getWidth(DataLayer::Y) + x); }
template <class T>
T YUVImage::getCbDataAt(int x, int y) { return this->getCbDataAt<T>(y*this->getWidth(DataLayer::Cb) + x); }
template <class T>
T YUVImage::getCrDataAt(int x, int y) { return this->getCrDataAt<T>(y*this->getWidth(DataLayer::Cr) + x); }

int YUVImage::getFormat() { return this->imgFormat; }

void YUVImage::setYDataAt(int x, int y, short d) { this->setYDataAt(y*this->getWidth(DataLayer::Y) + x, d); }
void YUVImage::setCbDataAt(int x, int y, short d) { this->setCbDataAt(y*this->getWidth(DataLayer::Cb) + x, d); }
void YUVImage::setCrDataAt(int x, int y, short d) { this->setCrDataAt(y*this->getWidth(DataLayer::Cr) + x, d); }
void YUVImage::setDataAt(int idx, short d) { this->data[idx] = (short)d; }
void YUVImage::setYDataAt(int idx, short d) { this->data[this->getOffset(DataLayer::Y) + idx] = (short)d; }
void YUVImage::setCbDataAt(int idx, short d) { this->data[this->getOffset(DataLayer::Cb) + idx] = (short)d; }
void YUVImage::setCrDataAt(int idx, short d) { this->data[this->getOffset(DataLayer::Cr) + idx] = (short)d; }

int YUVImage::getOffset(int opt) {
	switch(opt) {
		case DataLayer::Y  : return 0;
		case DataLayer::Cb : return this->imgWidth*this->imgHeight;
		case DataLayer::Cr : return (int)( (this->getScale()+1.0)/2.0 * this->imgWidth*this->imgHeight );
		default: return 0;
	}
}

int YUVImage::getWidth(int opt) {
	switch(opt) {
		case DataLayer::Y  : return this->imgWidth;
		case DataLayer::Cb :
		case DataLayer::Cr :
			if (this->imgFormat == FORMAT_4_4_4) return this->imgWidth;
			if (this->imgFormat == FORMAT_4_2_2) return this->imgWidth/2;
			if (this->imgFormat == FORMAT_4_2_0) return this->imgWidth/2;
		default: return 0;
	}
}

int YUVImage::getHeight(int opt) {
	switch(opt) {
		case DataLayer::Y  : return this->imgHeight;
		case DataLayer::Cb :
		case DataLayer::Cr :
			if (this->imgFormat == FORMAT_4_4_4) return this->imgHeight;
			if (this->imgFormat == FORMAT_4_2_2) return this->imgHeight;
			if (this->imgFormat == FORMAT_4_2_0) return this->imgHeight/2;
		default: return 0;
	}
}

short YUVImage::getMax(int opt) {
	short max = -1;
	if (opt == DataLayer::Y) {
		for (int i = 0; i < this->getDataSize(DataLayer::Y); i++)
			if (max < this->getYDataAt<short>(i)) max = this->getYDataAt<short>(i);
	} else if (opt == DataLayer::Cb) {
		for (int i = 0; i < this->getDataSize(DataLayer::Cb); i++)
			if (max < this->getCbDataAt<short>(i)) max = this->getCbDataAt<short>(i);
	} else if (opt == DataLayer::Cr) {
		for (int i = 0; i < this->getDataSize(DataLayer::Cr); i++)
			if (max < this->getCrDataAt<short>(i)) max = this->getCrDataAt<short>(i);
	}
	return max;
}

short YUVImage::getMin(int opt) {
	short min = -1;
	if (opt == DataLayer::Y) {
		for (int i = 0; i < this->getDataSize(DataLayer::Y); i++)
			if (min == -1 || min > this->getYDataAt<short>(i)) min = this->getYDataAt<short>(i);
	} else if (opt == DataLayer::Cb) {
		for (int i = 0; i < this->getDataSize(DataLayer::Cb); i++)
			if (min == -1 || min > this->getCbDataAt<short>(i)) min = this->getCbDataAt<short>(i);
	} else if (opt == DataLayer::Cr) {
		for (int i = 0; i < this->getDataSize(DataLayer::Cr); i++)
			if (min == -1 || min > this->getCrDataAt<short>(i)) min = this->getCrDataAt<short>(i);
	}
	return min;
}

double YUVImage::calPSNR(YUVImage* image) {
	if (image->getWidth(DataLayer::Y) != this->getWidth(DataLayer::Y) ||
		image->getHeight(DataLayer::Y) != this->getHeight(DataLayer::Y) ||
		image->getFormat() != this->getFormat()) {
		cout << "warning: The images cannot be compared." << endl;
		return 0;
	}
	double Y_MSE = 0, U_MSE = 0, V_MSE = 0;
	for (int i = 0; i < this->getDataSize(DataLayer::Y); i++) Y_MSE += pow( fabs(image->getYDataAt<short>(i) - this->getYDataAt<short>(i)) , 2);
	Y_MSE /= this->getDataSize(DataLayer::Y);
	for (int i = 0; i < this->getDataSize(DataLayer::Cb); i++) U_MSE += pow( fabs(image->getCbDataAt<short>(i) - this->getCbDataAt<short>(i)) , 2);
	U_MSE /= this->getDataSize(DataLayer::Cb);
	for (int i = 0; i < this->getDataSize(DataLayer::Cr); i++) V_MSE += pow( fabs(image->getCrDataAt<short>(i) - this->getCrDataAt<short>(i)) , 2);
	V_MSE /= this->getDataSize(DataLayer::Cr);
	// cout << "Y_MSE = " << Y_MSE << endl;
	// cout << "U_MSE = " << U_MSE << endl;
	// cout << "V_MSE = " << V_MSE << endl;
	double PSNR_Y = (Y_MSE == 0) ? 1000 : 10 * log(255*255 / Y_MSE)/log(10);
	double PSNR_U = (U_MSE == 0) ? 1000 : 10 * log(255*255 / U_MSE)/log(10);
	double PSNR_V = (V_MSE == 0) ? 1000 : 10 * log(255*255 / V_MSE)/log(10);
	double PSNR = (6*PSNR_Y + PSNR_U + PSNR_V) / 8;
	// cout << "PSNR_Y = " << PSNR_Y << endl;
	// cout << "PSNR_U = " << PSNR_U << endl;
	// cout << "PSNR_V = " << PSNR_V << endl;
	// cout << "PSNR = " << PSNR << endl;
	return PSNR;
}

bool YUVImage::replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

int YUVImage::getDataSize(int opt) { return this->getWidth(opt)*this->getHeight(opt); }
int YUVImage::getDataSize() { return this->dataSize; }

YUVImage::YUVImage(string path) { 
	this->imgWidth = -1; this->imgHeight = -1; this->imgFormat = -1; this->data = 0; this->dataSize = -1; this->imgPath = path; 
}
YUVImage::~YUVImage() { delete[] this->data; }

#endif