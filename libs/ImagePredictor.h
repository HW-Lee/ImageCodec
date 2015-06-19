//
//  ImagePredictor.h
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Created by HW Lee on 6/9/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#ifndef IMAGEPREDICTOR
#define IMAGEPREDICTOR

#include <iostream>
#include <math.h>
#include "YUVImage.h"

class ImagePredictor {
public:
	static YUVImage* predictResidual(YUVImage* originImg, int predictorNo, int* symbolNum);
	static YUVImage* predictResidual(YUVImage* originImg, int predictorNo);

	template <class T>
	static T getPrediction(T A, T B, T C, int predictorNo, T k);
	// predictor1: A (left-side)
	// predictor2: B (upper-side)
	// predictor3: C (left-upper-side)
	// predictor4: A + B - C
	// predictor5: A + (B - C)/2
	// predictor6: B + (A - C)/2
	// predictor7: (A + B)/2
	// predictor8: (3A + 3B - 2C)/4

private:
	template <class T>
	static T getPrediction(T A, T B, T C, int predictorNo);

	template <class T>
	static T circularPositive(T v, T k);

};

template <class T>
T ImagePredictor::getPrediction(T A, T B, T C, int predictorNo) {
	switch (predictorNo) {
		case 1: return A;
		case 2: return B;
		case 3: return C;
		case 4: return (A + B - C);
		case 5: return (A + (B - C)/2);
		case 6: return (B + (A - C)/2);
		case 7: return (A + B)/2;
		case 8: return (3*A + 3*B - 2*C)/4;
		default: return 0;
	}
}

template <class T>
T ImagePredictor::getPrediction(T A, T B, T C, int predictorNo, T k) {
	return circularPositive(getPrediction(A, B, C, predictorNo), k);
}

template <class T>
T ImagePredictor::circularPositive(T v, T k) {
	T vp = v;
	while (vp < 0) { vp += k; }
	return vp;
}

YUVImage* ImagePredictor::predictResidual(YUVImage* originImg, int predictorNo) {
	int* v = new int[3]; v[0] = 512; v[1] = 512; v[2] = 512;
	YUVImage* image = predictResidual(originImg, predictorNo, v);
	for (int i = 0; i < image->getDataSize(); i++) {
		if (image->getDataAt<short>(i) > 255) image->setDataAt(i, image->getDataAt<short>(i) - 512);
	}
	return image;
}

YUVImage* ImagePredictor::predictResidual(YUVImage* originImg, int predictorNo, int* symbolNum) {
	YUVImage* predictedImg = originImg->clone();
	short A, B, C;

	predictedImg->setYDataAt(0, 0, circularPositive(originImg->getYDataAt<short>(0, 0) - symbolNum[0]/2, symbolNum[0]) % symbolNum[0] );
	predictedImg->setCbDataAt(0, 0, circularPositive(originImg->getCbDataAt<short>(0, 0) - symbolNum[1]/2, symbolNum[1]) % symbolNum[1] );
	predictedImg->setCrDataAt(0, 0, circularPositive(originImg->getCrDataAt<short>(0, 0) - symbolNum[2]/2, symbolNum[2]) % symbolNum[2] );
	for (int i = 1; i < originImg->getWidth(DataLayer::Y); i++) {
		A = originImg->getYDataAt<short>(i-1, 0);
		predictedImg->setYDataAt(i, 0, circularPositive(originImg->getYDataAt<int>(i, 0) - A, symbolNum[0]) % symbolNum[0] );
		if (i < originImg->getWidth(DataLayer::Cb)) {
			A = originImg->getCbDataAt<short>(i-1, 0);
			predictedImg->setCbDataAt(i, 0, circularPositive(originImg->getCbDataAt<int>(i, 0) - A, symbolNum[1]) % symbolNum[1] );
		}
		if (i < originImg->getWidth(DataLayer::Cr)) {
			A = originImg->getCrDataAt<short>(i-1, 0);
			predictedImg->setCrDataAt(i, 0, circularPositive(originImg->getCrDataAt<int>(i, 0) - A, symbolNum[2]) % symbolNum[2] );
		}
	}
	for (int i = 1; i < originImg->getHeight(DataLayer::Y); i++) {
		B = originImg->getYDataAt<short>(0, i-1);
		predictedImg->setYDataAt(0, i, circularPositive(originImg->getYDataAt<int>(0, i) - B, symbolNum[0]) % symbolNum[0] );
		if (i < originImg->getHeight(DataLayer::Cb)) {
			B = originImg->getCbDataAt<short>(0, i-1);
			predictedImg->setCbDataAt(0, i, circularPositive(originImg->getCbDataAt<int>(0, i) - B, symbolNum[1]) % symbolNum[1] );
		}
		if (i < originImg->getHeight(DataLayer::Cr)) {
			B = originImg->getCrDataAt<short>(0, i-1);
			predictedImg->setCrDataAt(0, i, circularPositive(originImg->getCrDataAt<int>(0, i) - B, symbolNum[2]) % symbolNum[2] );
		}
	}

	for (int x = 1; x < originImg->getWidth(DataLayer::Y); x++) {
		for (int y = 1; y < originImg->getHeight(DataLayer::Y); y++) {
			A = originImg->getYDataAt<short>(x-1, y);
			B = originImg->getYDataAt<short>(x, y-1);
			C = originImg->getYDataAt<short>(x-1, y-1);
			predictedImg->setYDataAt(x, y, 
				( circularPositive(originImg->getYDataAt<int>(x, y) - getPrediction<int>(A, B, C, predictorNo), symbolNum[0]) ) % symbolNum[0]
			);
			if (x < originImg->getWidth(DataLayer::Cb) && y < originImg->getHeight(DataLayer::Cb)) {
				A = originImg->getCbDataAt<short>(x-1, y);
				B = originImg->getCbDataAt<short>(x, y-1);
				C = originImg->getCbDataAt<short>(x-1, y-1);
				predictedImg->setCbDataAt(x, y, 
					( circularPositive(originImg->getCbDataAt<int>(x, y) - getPrediction<int>(A, B, C, predictorNo), symbolNum[1]) ) % symbolNum[1]
				);
			}
			if (x < originImg->getWidth(DataLayer::Cr) && y < originImg->getHeight(DataLayer::Cr)) {
				A = originImg->getCrDataAt<short>(x-1, y);
				B = originImg->getCrDataAt<short>(x, y-1);
				C = originImg->getCrDataAt<short>(x-1, y-1);
				predictedImg->setCrDataAt(x, y, 
					( circularPositive(originImg->getCrDataAt<int>(x, y) - getPrediction<int>(A, B, C, predictorNo), symbolNum[2]) ) % symbolNum[2]
				);
			}
		}
	}

	return predictedImg;
}

#endif
