//
//  ImageCodec_Demo.cpp
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Created by HW Lee on 6/2/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>
#include "YUVImage.h"
#include "ImageCodec.h"
#include "PerformancePackage.h"

#define CONSTRAINT_BY_PSNR 0
#define CONSTRAINT_BY_BITRATE 1
#define SKIP_ENCODING -1

using namespace std;
int main(int argc, const char * argv[]) {

	int constWay = CONSTRAINT_BY_BITRATE;

	int opt = 3;
	string PATHS[] = {
		"./data/1_1536x1024.yuv",
		"./data/2_1024x768.yuv",
		"./data/3_1000x728.yuv",
		"./data/4_1000x1504.yuv",
		"./results/1_1536x1024/",
		"./results/2_1024x768/",
		"./results/3_1000x728/",
		"./results/4_1000x1504/",
	};
	int WIDTHS[] = {
		1536, 1024, 1000, 1000
	};
	int HEIGHTS[] = {
		1024, 768, 728, 1504
	};

	string path = PATHS[opt];
	int width = WIDTHS[opt]; int height = HEIGHTS[opt];
	YUVImage* img = YUVImage::import(path)
							->withFormat(YUVImage::FORMAT_4_2_0)
							->withSize(width, height);
	img->info();

	stringstream ss, ss1; 
	if (constWay == CONSTRAINT_BY_PSNR) {
		ss << PATHS[opt+4]; ss << "bitstream1.bin";
		ImageCodec::encodeTo(ss.str())->withImage(img)->withMinPSNR(28)->run();
		ss.str(""); ss.clear(); ss << PATHS[opt+4]; ss << "bitstream2.bin";
		ImageCodec::encodeTo(ss.str())->withImage(img)->withMinPSNR(37)->run();
		ss.str(""); ss.clear(); ss << PATHS[opt+4]; ss << "bitstream3.bin";
		ImageCodec::encodeTo(ss.str())->withImage(img)->withMinPSNR(47.5)->run();
	} else if (constWay == CONSTRAINT_BY_BITRATE) {
		ss << PATHS[opt+4]; ss << "bitstream1.bin";
		ImageCodec::encodeTo(ss.str())->withImage(img)->withMaxBitrate(0.55)->run();
		ss.str(""); ss.clear(); ss << PATHS[opt+4]; ss << "bitstream2.bin";
		ImageCodec::encodeTo(ss.str())->withImage(img)->withMaxBitrate(0.75)->run();
		ss.str(""); ss.clear(); ss << PATHS[opt+4]; ss << "bitstream3.bin";
		ImageCodec::encodeTo(ss.str())->withImage(img)->withMaxBitrate(1.0)->run();
	}

	ss.str(""); ss.clear(); ss << PATHS[opt+4] << "ParameterInfo.txt";
	ofstream s(ss.str().c_str(), ios::out | ios::app);
	PerformancePackage::getInstance(path)->info(s);
	s.close();

	YUVImage* img2;

	ss.str(""); ss.clear(); ss << PATHS[opt+4] << "bitstream1.bin"; ss1.str(""); ss1.clear(); ss1 << PATHS[opt+4] << "decompressed1.yuv";
	ImageCodec::decode(ss.str())->saveTo(ss1.str())->run();
	img2 = YUVImage::import(ss1.str())->withFormat(YUVImage::FORMAT_4_2_0)->withSize(width, height);

	cout << "PSNR = " << img->calPSNR(img2) << endl;
	cout << "BPP  = " << img->calBitrate(ss.str()) << endl;

	ss.str("");; ss.clear(); ss << PATHS[opt+4] << "bitstream2.bin"; ss1.str(""); ss1.clear(); ss1 << PATHS[opt+4] << "decompressed2.yuv";
	ImageCodec::decode(ss.str())->saveTo(ss1.str())->run();
	img2 = YUVImage::import(ss1.str())->withFormat(YUVImage::FORMAT_4_2_0)->withSize(width, height);

	cout << "PSNR = " << img->calPSNR(img2) << endl;
	cout << "BPP  = " << img->calBitrate(ss.str()) << endl;

	ss.str(""); ss.clear(); ss << PATHS[opt+4] << "bitstream3.bin"; ss1.str(""); ss1.clear(); ss1 << PATHS[opt+4] << "decompressed3.yuv";
	ImageCodec::decode(ss.str())->saveTo(ss1.str())->run();
	img2 = YUVImage::import(ss1.str())->withFormat(YUVImage::FORMAT_4_2_0)->withSize(width, height);

	cout << "PSNR = " << img->calPSNR(img2) << endl;
	cout << "BPP  = " << img->calBitrate(ss.str()) << endl;

	return 0;

	PerformancePackage* pkg = PerformancePackage::getInstance(path);
	for (int i = 0; i < pkg->getParametersCount(); i++) {
		cout << pkg->getParameterAt(i)->afterAdjust(YUVImage::FORMAT_4_2_0) << ", ";
		cout << pkg->getParameterAt(i)->PSNR << ";" << endl;
	}
}
