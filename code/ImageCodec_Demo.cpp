//
//  ImageCodec_Demo.cpp
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Created by HW Lee on 6/2/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#include <iostream>
#include <math.h>
#include "YUVImage.h"
#include "ImageCodec.h"
#include "PerformancePackage.h"

#define CONSTRAINT_BY_PSNR 0
#define CONSTRAINT_BY_BITRATE 1

using namespace std;
int main(int argc, const char * argv[]) {

	int constWay = CONSTRAINT_BY_BITRATE;

	int opt = 2;
	string PATHS[] = {
		"./data/1_1536x1024.yuv",
		"./data/2_1024x768.yuv",
		"./data/3_1000x728.yuv",
		"./data/4_1000x1504.yuv"
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

	if (constWay == CONSTRAINT_BY_PSNR) {
		ImageCodec::encodeTo("./results/bitstream1.bin")->withImage(img)->withMinPSNR(28)->run();
		ImageCodec::encodeTo("./results/bitstream2.bin")->withImage(img)->withMinPSNR(37)->run();
		ImageCodec::encodeTo("./results/bitstream3.bin")->withImage(img)->withMinPSNR(47.5)->run();
	} else {
		ImageCodec::encodeTo("./results/bitstream1.bin")->withImage(img)->withMaxBitrate(2.5)->run();
		ImageCodec::encodeTo("./results/bitstream2.bin")->withImage(img)->withMaxBitrate(3.5)->run();
		ImageCodec::encodeTo("./results/bitstream3.bin")->withImage(img)->withMaxBitrate(4.5)->run();
	}

	PerformancePackage::getInstance(path)->info();

	YUVImage* img2;

	ImageCodec::decode("./results/bitstream1.bin")->saveTo("./results/decompressed1.yuv")->run();
	img2 = YUVImage::import("./results/decompressed1.yuv")->withFormat(YUVImage::FORMAT_4_2_0)->withSize(width, height);

	cout << "PSNR = " << img->calPSNR(img2) << endl;
	cout << "BPP  = " << img->calBitrate("./results/bitstream1.bin") << endl;

	ImageCodec::decode("./results/bitstream2.bin")->saveTo("./results/decompressed2.yuv")->run();
	img2 = YUVImage::import("./results/decompressed2.yuv")->withFormat(YUVImage::FORMAT_4_2_0)->withSize(width, height);

	cout << "PSNR = " << img->calPSNR(img2) << endl;
	cout << "BPP  = " << img->calBitrate("./results/bitstream2.bin") << endl;

	ImageCodec::decode("./results/bitstream3.bin")->saveTo("./results/decompressed3.yuv")->run();
	img2 = YUVImage::import("./results/decompressed3.yuv")->withFormat(YUVImage::FORMAT_4_2_0)->withSize(width, height);

	cout << "PSNR = " << img->calPSNR(img2) << endl;
	cout << "BPP  = " << img->calBitrate("./results/bitstream3.bin") << endl;

	return 0;
}
