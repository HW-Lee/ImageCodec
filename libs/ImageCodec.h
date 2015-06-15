//
//  ImageCodec.h
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Modified by HW Lee on 6/12/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#ifndef IMAGECODEC
#define IMAGECODEC

#include <iostream>
#include <math.h>
#include "HuffmanTable.h"
#include "ImagePredictor.h"
#include "YUVImage.h"
#include "YUVImageFactory.h"
#include "BitWriter.h"
#include "BitReader.h"
#include "PerformancePackage.h"


using namespace std;
class ImageCodec {
public:
	static ImageCodec* encodeTo(string path);
	ImageCodec* withImage(YUVImage* image);
	ImageCodec* withMinPSNR(double minPSNR);
	ImageCodec* withMaxBitrate(double maxBitrate);

	static ImageCodec* decode(string path);
	ImageCodec* saveTo(string path);

	void run();

	static const int ENCODING;
	static const int DECODING;

private:
	ImageCodec(int mode, string path);
	~ImageCodec();

	int mode;
	double minPSNR;
	double maxBitrate;
	YUVImage* image;
	BitReader* reader;
	BitWriter* writer;
};

const int ImageCodec::ENCODING = 0;
const int ImageCodec::DECODING = 1;

ImageCodec::ImageCodec(int mode, string path) {
	this->mode = mode;
	if (mode == ImageCodec::ENCODING) { remove(path.c_str()); this->writer = BitWriter::open(path); }
	else { this->reader = BitReader::open(path); }
	this->minPSNR = 0;
	this->maxBitrate = 0;
}

ImageCodec* ImageCodec::encodeTo(string path) { ImageCodec* instance = new ImageCodec(ImageCodec::ENCODING, path); return instance; }
ImageCodec* ImageCodec::withImage(YUVImage* image) { this->image = image; return this; }
ImageCodec* ImageCodec::withMinPSNR(double minPSNR) { this->minPSNR = minPSNR; this->maxBitrate = -1; return this; }
ImageCodec* ImageCodec::withMaxBitrate(double maxBitrate) { this->maxBitrate = maxBitrate; this->minPSNR = -1; return this; }

ImageCodec* ImageCodec::decode(string path) { ImageCodec* instance = new ImageCodec(ImageCodec::DECODING, path); return instance; }
ImageCodec* ImageCodec::saveTo(string path) { remove(path.c_str()); this->writer = BitWriter::open(path); return this; }

void ImageCodec::run() {
	/*
	 *
	 *
	 *
	 *
	 *
	 * Decoding a bitstream
	 *
	 *
	 *
	 *
	 *
	 */
	if (this->mode == DECODING) {
		// get image information
		int width = this->reader->read<int>(16);
		int height = this->reader->read<int>(16);
		int format;
		if (this->reader->read() == 0) format = YUVImage::FORMAT_4_2_0;
		else if (this->reader->read() == 0) format = YUVImage::FORMAT_4_2_2;
		else format = YUVImage::FORMAT_4_4_4;

		YUVImage* outputImage = YUVImage::emptyImage("", width, height, YUVImage::FORMAT_4_4_4);

		// get symbol size of k-means clusters
		short *y_centers, *u_centers, *v_centers;
		int k_y = 1, k_u = 1, k_v = 1;
		int m_y = this->reader->read<int>(3);
		for (int i = 0; i < m_y; i++) k_y *= 2;

		y_centers = new short[k_y];
		for (int i = 0; i < k_y; i++) { y_centers[i] = this->reader->read<short>(8); }
		int m_u = this->reader->read<int>(3);
		for (int i = 0; i < m_u; i++) k_u *= 2;

		u_centers = new short[k_u];
		for (int i = 0; i < k_u; i++) { u_centers[i] = this->reader->read<short>(8); }
		int m_v = this->reader->read<int>(3);
		for (int i = 0; i < m_v; i++) k_v *= 2;

		v_centers = new short[k_v];
		for (int i = 0; i < k_v; i++) { v_centers[i] = this->reader->read<short>(8); }

		int k_predictorNo = this->reader->read<int>(3) + 1;
		long N_k = this->reader->read<long>(32);

		long y_truncator = 0; y_truncator = ~y_truncator; y_truncator <<= m_y; y_truncator = ~y_truncator;
		long u_truncator = 0; u_truncator = ~u_truncator; u_truncator <<= m_u; u_truncator = ~u_truncator;
		long v_truncator = 0; v_truncator = ~v_truncator; v_truncator <<= m_v; v_truncator = ~v_truncator;

		// read symbol values
		int* symbolValues = new int[N_k];
		for (int i = 0; i < N_k; i++) { symbolValues[i] = this->reader->read<int>(m_y + m_u + m_v); }

		// read entries table
		int m_table = this->reader->read<int>(8);
		int WL_max = this->reader->read<int>(m_table);
		int* entries = new int[WL_max];
		for (int i = 0; i < WL_max; i++) { entries[i] = this->reader->read<int>(m_table); }

		// construct number offset v.s. range offset table
		int* numOffsets = new int[WL_max];
		int* rangeOffsets = new int[WL_max];
		numOffsets[0] = 0; rangeOffsets[0] = 0;
		for (int i = 1; i <= WL_max; i++) {
			numOffsets[i] = numOffsets[i-1] + entries[i-1];
			rangeOffsets[i] = 2 * (rangeOffsets[i-1] + entries[i-1]);
		}

		numOffsets[0] = 0; rangeOffsets[0] = 0;

		// fetch bitstream to reconstruct the results of k-means clustering
		std::vector<char> v;
		int data_cnt = 0;
		while (data_cnt < outputImage->getDataSize(DataLayer::Y)) {
			if (this->reader->remains() == 0) break;
			v.push_back(this->reader->read());
			if (entries[v.size()-1] > 0) {
				int value = 0;
				for (int i = 0; i < v.size(); i++) value = value*2 + v[i];
				if ( value >= rangeOffsets[v.size()-1] && value < rangeOffsets[v.size()-1] + entries[v.size()-1] ) {
					int codeNumber = numOffsets[v.size()-1] + value - rangeOffsets[v.size()-1];
					int dataToWrite = symbolValues[codeNumber];
					int x = data_cnt % width;
					int y = data_cnt / width;
					short v_diff = dataToWrite & v_truncator;
					short u_diff = (dataToWrite >> m_v) & u_truncator;
					short y_diff = (dataToWrite >> (m_v+m_u)) & y_truncator;
					short A, B, C, D;
					if (x == 0 && y == 0) {
						outputImage->setYDataAt(x, y, (k_y/2 + y_diff) % k_y);
						outputImage->setCbDataAt(x, y, (k_u/2 + u_diff) % k_u);
						outputImage->setCrDataAt(x, y, (k_v/2 + v_diff) % k_v);
					} else if (x == 0 && y > 0) {
						B = outputImage->getYDataAt<short>(x, y-1);
						outputImage->setYDataAt(x, y, (B + y_diff) % k_y);
						B = outputImage->getCbDataAt<short>(x, y-1);
						outputImage->setCbDataAt(x, y, (B + u_diff) % k_u);
						B = outputImage->getCrDataAt<short>(x, y-1);
						outputImage->setCrDataAt(x, y, (B + v_diff) % k_v);
					} else if (y == 0 && x > 0) {
						A = outputImage->getYDataAt<short>(x-1, y);
						outputImage->setYDataAt(x, y, (A + y_diff) % k_y);
						A = outputImage->getCbDataAt<short>(x-1, y);
						outputImage->setCbDataAt(x, y, (A + u_diff) % k_u);
						A = outputImage->getCrDataAt<short>(x-1, y);
						outputImage->setCrDataAt(x, y, (A + v_diff) % k_v);
					} else {
						A = outputImage->getYDataAt<short>(x-1, y);
						B = outputImage->getYDataAt<short>(x, y-1);
						C = outputImage->getYDataAt<short>(x-1, y-1);
						D = ImagePredictor::getPrediction<short>(A, B, C, k_predictorNo, k_y);
						outputImage->setYDataAt(x, y, (D + y_diff) % k_y);

						A = outputImage->getCbDataAt<short>(x-1, y);
						B = outputImage->getCbDataAt<short>(x, y-1);
						C = outputImage->getCbDataAt<short>(x-1, y-1);
						D = ImagePredictor::getPrediction<short>(A, B, C, k_predictorNo, k_u);
						outputImage->setCbDataAt(x, y, (D + u_diff) % k_u);

						A = outputImage->getCrDataAt<short>(x-1, y);
						B = outputImage->getCrDataAt<short>(x, y-1);
						C = outputImage->getCrDataAt<short>(x-1, y-1);
						D = ImagePredictor::getPrediction<short>(A, B, C, k_predictorNo, k_v);
						outputImage->setCrDataAt(x, y, (D + v_diff) % k_v);
					}
					v.clear();
					data_cnt++;
				}
			}
			if (v.size() == WL_max) { cout << "error: WL_max(" << data_cnt << ")" << endl; break; }
		}

		// outputImage->transformToFormat(YUVImage::FORMAT_4_2_0)->exportTo("./results/clustered.yuv");

		// transform from class labels to class centers
		for (int i = 0; i < outputImage->getDataSize(DataLayer::Y); i++)
			outputImage->setYDataAt(i, y_centers[outputImage->getYDataAt<int>(i)]);
		for (int i = 0; i < outputImage->getDataSize(DataLayer::Cb); i++)
			outputImage->setCbDataAt(i, u_centers[outputImage->getCbDataAt<int>(i)]);
		for (int i = 0; i < outputImage->getDataSize(DataLayer::Cr); i++)
			outputImage->setCrDataAt(i, v_centers[outputImage->getCrDataAt<int>(i)]);

		outputImage->transformToFormat(format);

		// fetch bitstream to adjust the bias
		short Q_res = this->reader->read<short>(8);
		if (Q_res > 1) {
			YUVImage* adjImg = YUVImage::emptyImage("", width, height, format);

			int N_res = this->reader->read<int>(8);
			short* res_symbols = new short[N_res];
			for (int i = 0; i < N_res; i++) { res_symbols[i] = this->reader->read<short>(8); }
			int mres_table = this->reader->read<int>(8);
			int WLres_max = this->reader->read<int>(mres_table);
			int* res_entries = new int[WLres_max];
			for (int i = 0; i < WLres_max; i++) { res_entries[i] = this->reader->read<int>(mres_table); }

			int* res_numOffsets = new int[WLres_max];
			int* res_rangeOffsets = new int[WLres_max];
			res_numOffsets[0] = 0; res_rangeOffsets[0] = 0;
			for (int i = 1; i <= WLres_max; i++) {
				res_numOffsets[i] = res_numOffsets[i-1] + res_entries[i-1];
				res_rangeOffsets[i] = 2 * (res_rangeOffsets[i-1] + res_entries[i-1]);
			}
			res_numOffsets[0] = 0; res_rangeOffsets[0] = 0;

			int data_cnt = 0;
			std::vector<char> v;
			int tlength = outputImage->getDataSize(DataLayer::Y)+outputImage->getDataSize(DataLayer::Cb)+outputImage->getDataSize(DataLayer::Cr);
			while (data_cnt < tlength) {
				if (this->reader->remains() == 0) break;
				v.push_back(this->reader->read());
				if (res_entries[v.size()-1] > 0) {
					int value = 0;
					for (int i = 0; i < v.size(); i++) value = value*2 + v[i];
					if ( value >= res_rangeOffsets[v.size()-1] && value < res_rangeOffsets[v.size()-1] + res_entries[v.size()-1] ) {
						int codeNumber = res_numOffsets[v.size()-1] + value - res_rangeOffsets[v.size()-1];
						short dataToWrite = res_symbols[codeNumber];
						dataToWrite = (dataToWrite+1)/2*2*Q_res - (dataToWrite % 2);
						adjImg->setDataAt(data_cnt, dataToWrite);
						// if (data_cnt < 10) cout << "read[" << data_cnt << "] = " << dataToWrite << endl;
						v.clear();
						data_cnt++;
					}
				}
				if (v.size() == WLres_max) { cout << "error: WLres_max(" << data_cnt << ")" << endl; break; }
			}
			// adjImg->exportTo("./results/adjImg.yuv");
			outputImage = outputImage->add(adjImg->inv_diffReordering());
			delete adjImg;
			delete[] res_symbols; delete[] res_entries; delete[] res_numOffsets; delete[] res_rangeOffsets;
		}


		// export the image
		string path = this->writer->getPath();
		this->writer->close();
		this->reader->close();
		outputImage->exportTo(path);

		delete outputImage; delete[] y_centers; delete[] u_centers; delete[] v_centers;
		delete[] symbolValues; delete[] entries; delete[] numOffsets; delete[] rangeOffsets;

	/*
	 *
	 *
	 *
	 *
	 *
	 * Encoding an image with a specified min PSNR or max bitrate
	 *
	 *
	 *
	 *
	 *
	 */
	} else {
		// setup the range of m_y, m_u, m_v
		int M_Y_RANGE[] = {2, 3, 4, 5, 6}; int K_Y_RANGE[] = {4, 8, 16, 32, 64};
		int M_U_RANGE[] = {1, 2, 3, 4}; int K_U_RANGE[] = {2, 4, 8, 16};
		int M_V_RANGE[] = {1, 2, 3, 4}; int K_V_RANGE[] = {2, 4, 8, 16};
		int M_Y_RANGE_NUM = 5; int M_U_RANGE_NUM = 4; int M_V_RANGE_NUM = 4;

		int m_y; int k_y;
		int m_u; int k_u;
		int m_v; int k_v;

		int k_predictorNo = 8;

		// select the best set of parameters such that the PSNR is greater than specification and the compression ratio reaches the maximum
		if (PerformancePackage::getInstance(this->image->getPath())->getParametersCount() == 0) {
			// lossless bit rate
			int imageDataSize = this->image->getDataSize(DataLayer::Y);
			int l = this->image->getDataSize(DataLayer::Y)+this->image->getDataSize(DataLayer::Cb)+this->image->getDataSize(DataLayer::Cr);
			int* c = new int[3]; c[0] = 256; c[1] = 256; c[2] = 256;
			short* imgData = new short[l];
			CompressionParameters* cp = 
					CompressionParameters::Builder()
										->withArea(imageDataSize)
										->withMk_YUV(8, 8, 8)
										->withMk_TABLE(0)
										->withKmeansTableBitrate(0)
										->withResidualQuantizationConst(1)
										->withPSNR(1000);
			double br = 0;
			for (int ipid = 1; ipid <= 8; ipid++) {
				YUVImage* resImg = ImagePredictor::predictResidual(this->image, ipid, c);
				for (int i = 0; i < l; i++) imgData[i] = resImg->getDataAt<short>(i);
				HuffmanTable<short>* t = HuffmanTable<short>::initWithSize(256)->withData(imgData, l);
				if (br == 0 || br > t->bitrate()) {
					br = t->bitrate();
					cp->withPredictorID(ipid)->withKmeansTableSize(t->size())->withMaxWordLength(t->getMaxWordLength())->withAdjustBitrate(br);
				}

				delete resImg; delete t;
			}
			PerformancePackage::getInstance(this->image->getPath())->submit(cp);
			delete cp;
			delete[] c;
			for (int iy = 0; iy < M_Y_RANGE_NUM; iy++) {
				m_y = M_Y_RANGE[iy]; k_y = K_Y_RANGE[iy];
				cout << "tuning parameters: " << (double)iy / M_Y_RANGE_NUM * 100.0 << "%" << endl;
				if (k_y*2 > this->image->getMax(DataLayer::Y)-this->image->getMin(DataLayer::Y)) continue;
				for (int iu = 0; iu < M_U_RANGE_NUM; iu++) {
					m_u = M_U_RANGE[iu]; k_u = K_U_RANGE[iu];
					if (k_u*2 > this->image->getMax(DataLayer::Cb)-this->image->getMin(DataLayer::Cb)) continue;
					for (int iv = 0; iv < M_V_RANGE_NUM; iv++) {
						m_v = M_V_RANGE[iv]; k_v = K_V_RANGE[iv];
						if (k_v*2 > this->image->getMax(DataLayer::Cr)-this->image->getMin(DataLayer::Cr)) continue;
						// if (m_y != 5 || m_u != 2 || m_v != 1) continue;

						YUVImageFactory* factory = 
							YUVImageFactory::initWithImage(this->image)
											->useQuantization()
											->withYUVLevels(k_y, k_u, k_v)
											->compress();

						YUVImage* symbImg = factory->getSymbolImage()->transformToFormat(YUVImage::FORMAT_4_4_4);
						int symbolNum[3] = {k_y, k_u, k_v};
						YUVImage* kmeansPredictedResImg = ImagePredictor::predictResidual(symbImg, k_predictorNo, symbolNum);

						int* diff_data = new int[imageDataSize];

						for (int i = 0; i < imageDataSize; i++) {
							diff_data[i] = kmeansPredictedResImg->getYDataAt<int>(i); diff_data[i] <<= m_u;
							diff_data[i] |= kmeansPredictedResImg->getCbDataAt<int>(i); diff_data[i] <<= m_v;
							diff_data[i] |= kmeansPredictedResImg->getCrDataAt<int>(i);
						}

						HuffmanTable<int>* table = HuffmanTable<int>::initWithSize(1 << (m_y+m_u+m_v))->withData(diff_data, imageDataSize);

						int WL_max = table->getMaxWordLength();
						int max = WL_max;
						for (int i = 1; i <= WL_max; i++) {
							if (table->getNumEntriesWithWordLength(i) > max) max = table->getNumEntriesWithWordLength(i);
						}
						int m_table = 0;
						while (max > 0) { max >>= 1; m_table++; }
						int N_k = table->size();

						CompressionParameters* cp = 
							CompressionParameters::Builder()
												->withArea(imageDataSize)
												->withMk_YUV(m_y, m_u, m_v)
												->withPredictorID(k_predictorNo)
												->withKmeansTableSize(N_k)
												->withMk_TABLE(m_table)
												->withMaxWordLength(WL_max)
												->withKmeansTableBitrate(table->bitrate())
												->withAdjustBitrate(0)
												->withPSNR(this->image->calPSNR(factory->getImage()));

						PerformancePackage::getInstance(this->image->getPath())->submit(cp);

						// adjustment parameters tuning
						YUVImage* resImg = this->image->clone()->diff( factory->getImage() )->diffReordering();

						int res_min = resImg->getMax(DataLayer::Y);
						if (resImg->getMax(DataLayer::Cb) < res_min) res_min = resImg->getMax(DataLayer::Cb);
						if (resImg->getMax(DataLayer::Cr) < res_min) res_min = resImg->getMax(DataLayer::Cr);
						res_min = (res_min+1)/2;

						int tlength = resImg->getDataSize(DataLayer::Y)+resImg->getDataSize(DataLayer::Cb)+resImg->getDataSize(DataLayer::Cr);
						int* res_data = new int[tlength];

						if (res_min > 8) res_min >>= 2;
						while (res_min > 1) {
							YUVImage* dummy = resImg->clone();
							for (int i = 0; i < tlength; i++) {
								short v = resImg->getDataAt<short>(i);
								if (v == 0) { res_data[i] = 0; continue; }
								bool isPositive = (v % 2 == 0);
								v = v/(2*res_min); v = 2*v; if (!isPositive && v > 0) v--;
								res_data[i] = v;
								dummy->setDataAt(i, (v+1)/2*2*res_min - (v % 2));
							}

							dummy->inv_diffReordering();

							HuffmanTable<int>* resTable = HuffmanTable<int>::initWithSize(1 << 8)->withData(res_data, tlength);
							double adjustBitrate = resTable->bitrate();

							cp->withAdjustBitrate(adjustBitrate)->withPSNR(
								this->image->calPSNR(factory->getImage()->add(dummy))
							)->withResidualQuantizationConst(res_min);

							PerformancePackage::getInstance(this->image->getPath())->submit(cp);
							res_min >>= 1;
							delete dummy; delete resTable;
						}

						delete factory; delete symbImg; delete kmeansPredictedResImg; delete[] diff_data; delete table; delete cp;
						delete resImg; delete res_data;
					}
				}
			}
			PerformancePackage::getInstance(this->image->getPath())->anneal(this->image->getFormat());
			cout << "tuning parameters: 100%" << endl;
		}

		// use the parameters which result in the lowest bitrate or the greatest PSNR
		int searchStart = 1;
		PerformancePackage* perfpkg = PerformancePackage::getInstance(this->image->getPath());
		if (this->minPSNR > 0) {
			for (int i = 1; i < perfpkg->getParametersCount(); i++) {
				if (i == perfpkg->getParametersCount()-1 && perfpkg->getParameterAt(i)->PSNR > this->minPSNR) { searchStart = i; break; }
				else if (i == perfpkg->getParametersCount()-1) {}
				else if ( perfpkg->getParameterAt(i)->PSNR > this->minPSNR && perfpkg->getParameterAt(i+1)->PSNR < this->minPSNR ) {
					searchStart = i; break;
				}
			}
		} else {
			for (int i = 1; i < perfpkg->getParametersCount(); i++) {
				if (i == 1 && this->maxBitrate > perfpkg->getParameterAt(i)->afterAdjust(this->image->getFormat())) { searchStart = i; break; }
				if (i == perfpkg->getParametersCount()-1) { searchStart = i; break; }
				else if (perfpkg->getParameterAt(i+1)->afterAdjust(this->image->getFormat()) < this->maxBitrate
						&& perfpkg->getParameterAt(i)->afterAdjust(this->image->getFormat()) > this->maxBitrate) {
					searchStart = i+1; break;
				}
			}
		}

		// lossless coding (deprecated currently)
		if (searchStart == 0) {
			CompressionParameters* cp = PerformancePackage::getInstance(this->image->getPath())->getParameterAt(searchStart);
			int* c = new int[3]; c[0] = 256; c[1] = 256; c[2] = 256;
			YUVImage* resImg = ImagePredictor::predictResidual(this->image, cp->predictorID, c);

			int tlength = resImg->getDataSize(DataLayer::Y) + resImg->getDataSize(DataLayer::Cb) + resImg->getDataSize(DataLayer::Cr);
			short* data = new short[tlength];
			for (int i = 0; i < tlength; i++) { data[i] = resImg->getDataAt<short>(i); }

			HuffmanTable<short>* t = HuffmanTable<short>::initWithSize(256)->withData(data, tlength);

			delete resImg; delete cp; delete t;
			this->writer->close();
			return;
		}

		// find the best predictor
		CompressionParameters* cp = PerformancePackage::getInstance(this->image->getPath())->getParameterAt(searchStart);
		m_y = cp->mk_y; m_u = cp->mk_u; m_v = cp->mk_v;
		k_y = (1 << m_y); k_u = (1 << m_u); k_v = (1 << m_v);

		int bestPredictorNo = 0; double bestBitrate = 0;
		for (int ipid = 1; ipid <= 8; ipid++) {
			YUVImageFactory* factory = 
				YUVImageFactory::initWithImage(this->image)
								->useQuantization()
								->withYUVLevels(k_y, k_u, k_v)
								->compress();

			YUVImage* symbImg = factory->getSymbolImage()->transformToFormat(YUVImage::FORMAT_4_4_4);
			int symbolNum[3] = {k_y, k_u, k_v};
			YUVImage* kmeansPredictedResImg = ImagePredictor::predictResidual(symbImg, ipid, symbolNum);

			int imageDataSize = this->image->getDataSize(DataLayer::Y);
			int* diff_data = new int[imageDataSize];

			for (int i = 0; i < imageDataSize; i++) {
				diff_data[i] = kmeansPredictedResImg->getYDataAt<int>(i); diff_data[i] <<= m_u;
				diff_data[i] |= kmeansPredictedResImg->getCbDataAt<int>(i); diff_data[i] <<= m_v;
				diff_data[i] |= kmeansPredictedResImg->getCrDataAt<int>(i);
			}

			HuffmanTable<int>* kmeansTable = HuffmanTable<int>::initWithSize(1 << (m_y+m_u+m_v))->withData(diff_data, imageDataSize);

			if (bestPredictorNo == 0) { bestPredictorNo = 1; bestBitrate = kmeansTable->bitrate(); }
			else if (kmeansTable->bitrate() < bestBitrate) { bestPredictorNo = ipid; bestBitrate = kmeansTable->bitrate(); }
			// cout << ipid << ": " << kmeansTable->bitrate() << endl;
			cout << "tuning predictor: " << ipid*12.5 << "%" << endl;
			
			delete factory; delete symbImg; delete kmeansPredictedResImg; delete[] diff_data; delete kmeansTable;
		}

		// cout << "bestPredictorNo = " << bestPredictorNo << ", bestBitrate = " << bestBitrate << endl;

		YUVImageFactory* factory = YUVImageFactory::initWithImage(this->image)
													->useQuantization()
													->withYUVLevels(k_y, k_u, k_v)
													->compress();

		
		YUVImage* symbImg = factory->getSymbolImage()->transformToFormat(YUVImage::FORMAT_4_4_4);
		int symbolNum[3] = {k_y, k_u, k_v};
		YUVImage* kmeansPredictedResImg = ImagePredictor::predictResidual(symbImg, bestPredictorNo, symbolNum);

		int imageDataSize = this->image->getDataSize(DataLayer::Y);
		int* diff_data = new int[imageDataSize];

		for (int i = 0; i < imageDataSize; i++) {
			diff_data[i] = kmeansPredictedResImg->getYDataAt<int>(i); diff_data[i] <<= m_u;
			diff_data[i] |= kmeansPredictedResImg->getCbDataAt<int>(i); diff_data[i] <<= m_v;
			diff_data[i] |= kmeansPredictedResImg->getCrDataAt<int>(i);
		}

		HuffmanTable<int>* kmeansTable = HuffmanTable<int>::initWithSize(1 << (m_y+m_u+m_v))->withData(diff_data, imageDataSize);

		int WL_max = kmeansTable->getMaxWordLength();
		int max = WL_max;
		for (int i = 1; i <= WL_max; i++) { if (kmeansTable->getNumEntriesWithWordLength(i) > max) max = kmeansTable->getNumEntriesWithWordLength(i); }
		int m_table = 0;
		while (max > 0) { max >>= 1; m_table++; }

		// compress the image
		this->writer->write<int>(this->image->getWidth(DataLayer::Y), 16);
		this->writer->write<int>(this->image->getHeight(DataLayer::Y), 16);
		if (this->image->getFormat() == YUVImage::FORMAT_4_2_0) { this->writer->write<short>(0, 1); }
		else if (this->image->getFormat() == YUVImage::FORMAT_4_2_2) { this->writer->write<short>(2, 2); }
		else { this->writer->write<short>(3, 2); }

		this->writer->write<int>(m_y, 3);
		for (int i = 0; i < k_y; i++) { this->writer->write<short>(factory->getYSymbolAt(i), 8); }
		this->writer->write<int>(m_u, 3);
		for (int i = 0; i < k_u; i++) { this->writer->write<short>(factory->getUSymbolAt(i), 8); }
		this->writer->write<int>(m_v, 3);
		for (int i = 0; i < k_v; i++) { this->writer->write<short>(factory->getVSymbolAt(i), 8); }

		this->writer->write<short>(bestPredictorNo-1, 3);

		this->writer->write<int>(kmeansTable->size(), 32);
		for (int i = 0; i < kmeansTable->size(); i++) { this->writer->write<int>(kmeansTable->getSymbolAt(i)->getValue(), m_y+m_u+m_v); }

		this->writer->write<int>(m_table, 8);
		this->writer->write<int>(kmeansTable->getMaxWordLength(), m_table);
		for (int i = 1; i <= kmeansTable->getMaxWordLength(); i++) { this->writer->write<int>(kmeansTable->getNumEntriesWithWordLength(i), m_table); }

		for (int i = 0; i < imageDataSize; i++) {
			bool flag = false;
			for (int j = 0; j < kmeansTable->size(); j++)
				if (diff_data[i] == kmeansTable->getSymbolAt(j)->getValue()) {
					flag = true; this->writer->write(kmeansTable->getSymbolAt(j)->getWord()); break;
				}
			if (!flag) { this->writer->close(); cout << "error at diff_data[" << i << "]: " << diff_data[i] << endl; return; }
		}

		// kmeansTable->info();

		// coding adjustment data
		if (cp->isWithAdjust) {
			YUVImage* resImg = this->image->clone()->diff( factory->getImage() )->diffReordering();
			int Q_res = cp->Q_res;

			int tlength = resImg->getDataSize(DataLayer::Y)+resImg->getDataSize(DataLayer::Cb)+resImg->getDataSize(DataLayer::Cr);
			short* res_data = new short[tlength];
			for (int i = 0; i < tlength; i++) {
				short v = resImg->getDataAt<short>(i);
				if (v == 0) { res_data[i] = 0; continue; }
				bool isPositive = (v % 2 == 0);
				v = v/(2*Q_res); v = 2*v; if (!isPositive && v > 0) v--;
				res_data[i] = v;
			}
			for (int i = 0; i < tlength; i++) { resImg->setDataAt(i, (res_data[i]+1)/2*2*Q_res - (res_data[i] % 2)); }

			// resImg->exportTo("./results/resImg.yuv");
			// factory->getSymbolImage()->exportTo("./results/factory.yuv");

			// cout << "PSNR = " << this->image->calPSNR(factory->getImage()) << endl;
			// cout << "PSNR = " << this->image->calPSNR(factory->getImage()->add(resImg->inv_diffReordering())) << endl;

			HuffmanTable<short>* resTable = HuffmanTable<short>::initWithSize(1 << 8)->withData(res_data, tlength);

			this->writer->write<int>(cp->Q_res, 8);
			this->writer->write<int>(resTable->size(), 8);
			for (int i = 0; i < resTable->size(); i++) { this->writer->write<int>(resTable->getSymbolAt(i)->getValue(), 8); }

			int WLres_max = resTable->getMaxWordLength();
			int max = WLres_max;
			for (int i = 1; i <= WLres_max; i++) {
				if (max < resTable->getNumEntriesWithWordLength(i)) max = resTable->getNumEntriesWithWordLength(i);
			}
			int mres_table = 0;
			while (max > 0) { max >>= 1; mres_table++; }

			this->writer->write<int>(mres_table, 8);
			this->writer->write<int>(WLres_max, mres_table);
			for (int i = 1; i <= WLres_max; i++) { this->writer->write<int>(resTable->getNumEntriesWithWordLength(i), mres_table); }

			// resTable->info();

			for (int i = 0; i < tlength; i++) {
				bool flag = false;
				for (int j = 0; j < resTable->size(); j++)
					if (res_data[i] == resTable->getSymbolAt(j)->getValue()) {
						flag = true; this->writer->write(resTable->getSymbolAt(j)->getWord()); break;
					}
				if (!flag) { this->writer->close(); cout << "error at res_data[" << i << "]: " << res_data[i] << endl; return; }
				// if (i < 10) cout << "res_data[" << i << "]: " << res_data[i] << endl;
			}

			delete resImg; delete[] res_data; delete resTable;
		}

		this->writer->close();

		delete factory; delete symbImg; delete kmeansPredictedResImg; delete[] diff_data; delete kmeansTable;
	}
}

#endif