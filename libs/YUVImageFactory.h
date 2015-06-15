//
//  YUVImageFactory.h
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Created by HW Lee on 6/7/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#ifndef YUVIMAGEFACTORY
#define YUVIMAGEFACTORY

#include <iostream>
#include "YUVImage.h"
#include "KmeansFactory.h"
#include "HuffmanTable.h"

class YUVImageFactory {
public:
	~YUVImageFactory();
	static YUVImageFactory* initWithImage(YUVImage* image);
	YUVImageFactory* useQuantization();
	YUVImageFactory* withYUVLevels(int Ylevel, int Ulevel, int Vlevel);
	YUVImageFactory* withLevels(int level);
	YUVImageFactory* compress();

	YUVImage* getImage();
	YUVImage* getSymbolImage();
	YUVImage* getResidualImage();
	int getYLevel();
	int getULevel();
	int getVLevel();
	short getYSymbolAt(int idx);
	short getUSymbolAt(int idx);
	short getVSymbolAt(int idx);

private:
	YUVImageFactory(YUVImage* image);
	void init();

	YUVImage* image;
	YUVImage* originImg;
	bool isQuantized;
	int Ylevel, Ulevel, Vlevel, level;
	short* YSymbols;
	short* USymbols;
	short* VSymbols;

};

YUVImageFactory* YUVImageFactory::initWithImage(YUVImage* image) { return new YUVImageFactory(image->clone()); }
YUVImageFactory* YUVImageFactory::useQuantization() { this->isQuantized = true; return this; }
YUVImageFactory* YUVImageFactory::withYUVLevels(int Ylevel, int Ulevel, int Vlevel) { 
	this->Ylevel = Ylevel; this->Ulevel = Ulevel; this->Vlevel = Vlevel; return this; 
}
YUVImageFactory* YUVImageFactory::withLevels(int level) { this->level = level; return this->withYUVLevels(0, 0, 0); }

int YUVImageFactory::getYLevel() { return this->Ylevel; }
int YUVImageFactory::getULevel() { return this->Ulevel; }
int YUVImageFactory::getVLevel() { return this->Vlevel; }
short YUVImageFactory::getYSymbolAt(int idx) { return this->YSymbols[idx]; }
short YUVImageFactory::getUSymbolAt(int idx) { return this->USymbols[idx]; }
short YUVImageFactory::getVSymbolAt(int idx) { return this->VSymbols[idx]; }
YUVImage* YUVImageFactory::getSymbolImage() { return this->image->clone(); }

YUVImage* YUVImageFactory::getImage() {
	if (!this->isQuantized) return this->image->clone();
	YUVImage* imageClone = this->image->clone();
	for (int i = 0; i < imageClone->getDataSize(DataLayer::Y); i++) { imageClone->setYDataAt(i, YSymbols[imageClone->getYDataAt<short>(i)]); }
	for (int i = 0; i < imageClone->getDataSize(DataLayer::Cb); i++) { imageClone->setCbDataAt(i, USymbols[imageClone->getCbDataAt<short>(i)]); }
	for (int i = 0; i < imageClone->getDataSize(DataLayer::Cr); i++) { imageClone->setCrDataAt(i, VSymbols[imageClone->getCrDataAt<short>(i)]); }
	return imageClone;
}

YUVImageFactory* YUVImageFactory::compress() {
	if (!this->isQuantized) return this;
	if (this->Ylevel == this->Ulevel && this->Ulevel == this->Vlevel && this->Ylevel == 0) {
		KmeansFactory<long>* factory = KmeansFactory<long>::getInstance();
		this->image->transformToFormat(YUVImage::FORMAT_4_4_4);
		long* data = new long[this->image->getDataSize(DataLayer::Y)];
		for (int i = 0; i < this->image->getDataSize(DataLayer::Y); i++) data[i] = 0;
		for (int i = 0; i < this->image->getDataSize(DataLayer::Y); i++)
			data[i] = ( (this->image->getYDataAt<int>(i) << 16) | (this->image->getCbDataAt<int>(i) << 8) | this->image->getCrDataAt<int>(i) );

		KmeansInfo<long, long>* info = factory->withRawData(data, this->image->getDataSize(DataLayer::Y))->clusterInto(this->level)->run<long>();
		this->Ylevel = 1; this->Ulevel = 1; this->Vlevel = info->nCenters;
		this->YSymbols = new short[info->nCenters]; this->USymbols = new short[info->nCenters]; this->VSymbols = new short[info->nCenters];
		for (int i = 0; i < info->nCenters; i++) {
			this->YSymbols[i] = ((info->centers[i] >> 16) & 0x0000ffff);
			this->USymbols[i] = ((info->centers[i] >> 8) & 0x0000ffff);
			this->VSymbols[i] = (info->centers[i] & 0x0000ffff);
		}
		for (int i = 0; i < info->nData; i++) {
			this->image->setYDataAt(i, info->dataSymbols[i] & 0x0000ffff);
			this->image->setYDataAt(i, info->dataSymbols[i] & 0x0000ffff);
			this->image->setYDataAt(i, info->dataSymbols[i] & 0x0000ffff);
		}

		delete info; delete[] data;
		return this;
	} else {
		KmeansFactory<char>* factory = KmeansFactory<char>::getInstance();
		KmeansInfo<char, char>* info;
		char* data = new char[this->image->getDataSize(DataLayer::Y)];
		for (int i = 0; i < this->image->getDataSize(DataLayer::Y); i++) data[i] = this->image->getYDataAt<char>(i);
		info = factory->withRawData(data, this->image->getDataSize(DataLayer::Y))->clusterInto(this->Ylevel)->run<char>();
		this->Ylevel = info->nCenters;
		this->YSymbols = new short[info->nCenters];
		for (int i = 0; i < info->nCenters; i++) this->YSymbols[i] = (short)(info->centers[i] & 0x00ff);
		for (int i = 0; i < info->nData; i++) this->image->setYDataAt(i, info->dataSymbols[i]);

		for (int i = 0; i < this->image->getDataSize(DataLayer::Cb); i++) data[i] = this->image->getCbDataAt<char>(i);
		info = factory->withRawData(data, this->image->getDataSize(DataLayer::Cb))->clusterInto(this->Ulevel)->run<char>();
		this->Ulevel = info->nCenters;
		this->USymbols = new short[info->nCenters];
		for (int i = 0; i < info->nCenters; i++) this->USymbols[i] = (short)(info->centers[i] & 0x00ff);
		for (int i = 0; i < info->nData; i++) this->image->setCbDataAt(i, info->dataSymbols[i]);

		for (int i = 0; i < this->image->getDataSize(DataLayer::Cr); i++) data[i] = this->image->getCrDataAt<char>(i);
		info = factory->withRawData(data, this->image->getDataSize(DataLayer::Cr))->clusterInto(this->Vlevel)->run<char>();
		this->Vlevel = info->nCenters;
		this->VSymbols = new short[info->nCenters];
		for (int i = 0; i < info->nCenters; i++) this->VSymbols[i] = (short)(info->centers[i] & 0x00ff);
		for (int i = 0; i < info->nData; i++) this->image->setCrDataAt(i, info->dataSymbols[i]);

		delete info; delete[] data;
		return this;
	}
}

YUVImage* YUVImageFactory::getResidualImage() {
	YUVImage* residueImg = this->getSymbolImage()->transformToFormat(YUVImage::FORMAT_4_4_4);
	for (int i = 0; i < residueImg->getDataSize(DataLayer::Y); i++) {
		short d = (this->originImg->getYDataAt<short>(i) - residueImg->getYDataAt<short>(i) + 256) % 256;
		residueImg->setYDataAt(i, d);
	}
	for (int i = 0; i < residueImg->getDataSize(DataLayer::Cb); i++) {
		short d = (this->originImg->getCbDataAt<short>(i) - residueImg->getCbDataAt<short>(i) + 256) % 256;
		residueImg->setCbDataAt(i, d);
	}
	for (int i = 0; i < residueImg->getDataSize(DataLayer::Cr); i++) {
		short d = (this->originImg->getCrDataAt<short>(i) - residueImg->getCrDataAt<short>(i) + 256) % 256;
		residueImg->setCrDataAt(i, d);
	}
	return residueImg;
}

YUVImageFactory::YUVImageFactory(YUVImage* image) { this->image = image; this->originImg = image->clone(); this->init(); }
void YUVImageFactory::init() {
	this->isQuantized = false;
	this->Ylevel = -1; this->Ulevel = -1; this->Vlevel = -1; this->level = -1;
}

YUVImageFactory::~YUVImageFactory() { 
	delete image; delete[] YSymbols; delete[] USymbols; delete[] VSymbols; 
}


#endif
