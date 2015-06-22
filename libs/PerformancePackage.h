//
//  PerformancePackage.h
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Created by HW Lee on 6/12/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#ifndef PERFORMANCEPACKAGE
#define PERFORMANCEPACKAGE

#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
#include <sstream>
#include "YUVImage.h"
#include "BitReader.h"
#include "BitWriter.h"

using namespace std;
class CompressionParameters {
public:
	static CompressionParameters* Builder();
	CompressionParameters* withArea(long area);
	CompressionParameters* withDownSampleScale(int wscale, int hscale);
	CompressionParameters* withMk_YUV(int m_y, int m_u, int m_v);
	CompressionParameters* withPredictorID(int predictorID);
	CompressionParameters* withKmeansTableSize(int N_k);
	CompressionParameters* withMk_TABLE(int m_table);
	CompressionParameters* withMaxWordLength(int WL_max);
	CompressionParameters* withKmeansTableBitrate(double kmeansTableBitrate);
	CompressionParameters* withAdjustBitrate(double adjustBitrate);
	CompressionParameters* withResidualQuantizationConst(int Q_res);

	CompressionParameters* withPSNR(double PSNR);
	CompressionParameters* withAdjust(bool isWithAdjust);
	CompressionParameters* clone();

	void info();
	void info(ofstream &s);

	long bitsCount();
	double afterAdjust(int format);

	long imageArea;
	int wscale;
	int hscale;
	int mk_y, mk_u, mk_v;
	int predictorID;
	int N_k;
	int mk_table;
	int WL_max;
	double kmeansTableBitrate;
	double adjustBitrate;
	int Q_res;

	double PSNR;
	bool isWithAdjust;

private:
	CompressionParameters();

};

CompressionParameters::CompressionParameters() { withAdjust(false); }
CompressionParameters* CompressionParameters::Builder() { return new CompressionParameters(); }
CompressionParameters* CompressionParameters::withArea(long area) { this->imageArea = area; return this; }
CompressionParameters* CompressionParameters::withDownSampleScale(int wscale, int hscale) { this->wscale = wscale; this->hscale = hscale; return this; }
CompressionParameters* CompressionParameters::withMk_YUV(int m_y, int m_u, int m_v) { this->mk_y = m_y; this->mk_u = m_u; this->mk_v = m_v; return this; }
CompressionParameters* CompressionParameters::withPredictorID(int predictorID) { this->predictorID = predictorID; return this; }
CompressionParameters* CompressionParameters::withKmeansTableSize(int N_k) { this->N_k = N_k; return this; }
CompressionParameters* CompressionParameters::withMk_TABLE(int m_table) { this->mk_table = m_table; return this; }
CompressionParameters* CompressionParameters::withMaxWordLength(int WL_max) { this->WL_max = WL_max; return this; }
CompressionParameters* CompressionParameters::withKmeansTableBitrate(double kmeansTableBitrate) { this->kmeansTableBitrate = kmeansTableBitrate; return this; }
CompressionParameters* CompressionParameters::withAdjustBitrate(double adjustBitrate) { this->adjustBitrate = adjustBitrate; return this; }
CompressionParameters* CompressionParameters::withAdjust(bool isWithAdjust) { this->isWithAdjust = isWithAdjust; return this; }
CompressionParameters* CompressionParameters::withResidualQuantizationConst(int Q_res) { this->Q_res = Q_res; return withAdjust(true); }

CompressionParameters* CompressionParameters::withPSNR(double PSNR) { this->PSNR = PSNR; return this; }
CompressionParameters* CompressionParameters::clone() {
	return Builder()->withArea(imageArea)
					->withDownSampleScale(wscale, hscale)
					->withMk_YUV(mk_y, mk_u, mk_v)
					->withPredictorID(predictorID)
					->withKmeansTableSize(N_k)
					->withMk_TABLE(mk_table)
					->withMaxWordLength(WL_max)
					->withKmeansTableBitrate(kmeansTableBitrate)
					->withAdjustBitrate(adjustBitrate)
					->withResidualQuantizationConst(Q_res)
					->withPSNR(PSNR)
					->withAdjust(isWithAdjust);
}

void CompressionParameters::info() {
	cout << "Area: " << this->imageArea << ", ";
	cout << "DownSampleScale: (" << wscale << ", " << hscale << ")" << ", ";
	cout << "(mk_y, mk_u, mk_v) = (" << mk_y << ", " << mk_u << ", " << mk_v << ")" << endl;
	cout << "PredictorID: " << predictorID << ", " << "KmeansTableSize: " << N_k << ", ";
	cout << "mk_table: " << mk_table << ", " << "WL_max: " << WL_max << endl;
	cout << "kmeansTableBitrate: " << kmeansTableBitrate << ", PSNR: " << PSNR << ", ";
	cout << "AdjustBitrate: " << adjustBitrate << endl;
	cout << "withAdjustBitrate: ";
	for (int i = 0; i < 3; i++) { cout << afterAdjust(i); if (i < 2) cout << ", "; }
	cout << endl;
	cout << "isWithAdjust: " << ((isWithAdjust) ? "Y":"N") << endl;
	if (isWithAdjust) { cout << "Q_res: " << Q_res << endl; }
}

void CompressionParameters::info(ofstream &s) {
	s << "Area: " << this->imageArea << ", ";
	s << "DownSampleScale: (" << wscale << ", " << hscale << ")" << ", ";
	s << "(mk_y, mk_u, mk_v) = (" << mk_y << ", " << mk_u << ", " << mk_v << ")" << endl;
	s << "PredictorID: " << predictorID << ", " << "KmeansTableSize: " << N_k << ", ";
	s << "mk_table: " << mk_table << ", " << "WL_max: " << WL_max << endl;
	s << "kmeansTableBitrate: " << kmeansTableBitrate << ", PSNR: " << PSNR << ", ";
	s << "AdjustBitrate: " << adjustBitrate << endl;
	s << "withAdjustBitrate: ";
	for (int i = 0; i < 3; i++) { s << afterAdjust(i); if (i < 2) s << ", "; }
	s << endl;
	s << "isWithAdjust: " << ((isWithAdjust) ? "Y":"N") << endl;
	if (isWithAdjust) { s << "Q_res: " << Q_res << endl; }
}

long CompressionParameters::bitsCount() {
	long cnt = 0;
	int k_y = (1 << this->mk_y), k_u = (1 << this->mk_u), k_v = (1 << this->mk_v);
	cnt += 16*2; // image size
	cnt += 2; // format
	cnt += 3*3; // m_Y, m_U, m_V
	cnt += 8*(k_y + k_u + k_v); // centers of Y, U, and V
	cnt += 3; // predictor ID
	cnt += 32; // # of residual combinations of cluster labels predicted residual
	cnt += this->N_k*(this->mk_y + this->mk_u + this->mk_v); // residual combinations of cluster labels predicted residual
	cnt += 8; // m_table
	cnt += this->mk_table; // max word length
	cnt += this->WL_max * this->mk_table; // entries
	cnt += round(this->kmeansTableBitrate * this->imageArea); // bitstream of residual combinations
	return cnt;
}

double CompressionParameters::afterAdjust(int format) {
	if (format == YUVImage::FORMAT_4_4_4) return ( adjustBitrate*3.0 + kmeansTableBitrate ) / (wscale*hscale);
	else if (format == YUVImage::FORMAT_4_2_2) return ( adjustBitrate*2.0 + kmeansTableBitrate ) / (wscale*hscale);
	else return ( adjustBitrate*1.5 + kmeansTableBitrate ) / (wscale*hscale);
}

class PerformancePackage {
public:
	static PerformancePackage* getInstance(string name);
	void clear();
	void submit(CompressionParameters* par);
	void anneal(int opt);
	void info();
	void info(ofstream &s);

	static void save();
	static void load(string name);

	string getName();
	int getParametersCount();
	CompressionParameters* getParameterAt(int idx);

private:
	PerformancePackage(string name);

	string buildPath();

	static PerformancePackage** instances;
	static int curSize;
	static int curMaxSize;
	std::vector<CompressionParameters*> parameters;
	string name;

	static const string CACHE_ROOT;
	static const string CACHE_POSTFIX;
	static const long TRUNCATED_DEC;
};

PerformancePackage** PerformancePackage::instances = new PerformancePackage*[10];
int PerformancePackage::curSize = 0;
int PerformancePackage::curMaxSize = 10;
const string PerformancePackage::CACHE_ROOT = "./libs/PerformancePackageCache/";
const string PerformancePackage::CACHE_POSTFIX = ".pp";
const long PerformancePackage::TRUNCATED_DEC = 10000000;

PerformancePackage::PerformancePackage(string name) { this->name = name; };
string PerformancePackage::getName() { return this->name; }
int PerformancePackage::getParametersCount() { return this->parameters.size(); }
CompressionParameters* PerformancePackage::getParameterAt(int idx) { return this->parameters[idx]; }

PerformancePackage* PerformancePackage::getInstance(string name) {
	for (int i = 0; i < curSize; i++) {
		if (instances[i]->getName() == name) { /*cout << "Hit on " << i << endl;*/ return instances[i]; }
	}
	if (curSize == curMaxSize) {
		curMaxSize += 10;
		PerformancePackage** newInstances = new PerformancePackage*[curMaxSize];
		for (int i = 0; i < curSize; i++) newInstances[i] = instances[i];
		delete[] instances;
		instances = newInstances;
		/*cout << "expand the size to " << curMaxSize << endl;*/
	}
	instances[curSize++] = new PerformancePackage(name);
	/*cout << "Miss on " << name << ", create a new object." << endl;*/
	return instances[curSize-1];
}
void PerformancePackage::clear() { this->parameters.clear(); }

void PerformancePackage::submit(CompressionParameters* par) {
	if (this->parameters.size() == 0) { this->parameters.push_back(par->clone()); return; }
	int searchIdx = 0;
	for (int i = 0; i < this->parameters.size(); i++) {
		if (this->parameters[i]->PSNR < par->PSNR) { this->parameters.insert(this->parameters.begin()+i, par->clone()); return; }
	}
	this->parameters.push_back(par->clone());
}

void PerformancePackage::anneal(int opt) {
	int idx = 0; double minBits = 0;
	while (idx < this->parameters.size()) {
		if (minBits == 0) { minBits = this->parameters[idx]->afterAdjust(opt); idx++; continue; }
		if (minBits < this->parameters[idx]->afterAdjust(opt)) { this->parameters.erase(this->parameters.begin()+idx); }
		else { minBits = this->parameters[idx]->afterAdjust(opt); idx++; }
	}
}

void PerformancePackage::info() {
	for (int i = 0; i < this->parameters.size(); i++) {
		cout << "parameters[" << i << "]: " << endl;
		this->parameters[i]->info();
		cout << endl << "---------------------------------------" << endl;
	}
}

void PerformancePackage::info(ofstream &s) {
	for (int i = 0; i < this->parameters.size(); i++) {
		s << "parameters[" << i << "]: " << endl;
		this->parameters[i]->info(s);
		s << endl << "---------------------------------------" << endl;
	}
}

void PerformancePackage::save() {
	for (int pkgIdx = 0; pkgIdx < curSize; pkgIdx++) {
		PerformancePackage* pkg = instances[pkgIdx];
		cout << "save to " << pkg->buildPath() << endl;
		BitWriter* w = BitWriter::open(pkg->buildPath());
		w->write<int>(pkg->parameters.size(), 16);
		for (int i = 0; i < pkg->parameters.size(); i++) {
			CompressionParameters* cp = pkg->parameters[i];
			w->write<int>(cp->imageArea, 32);
			w->write<int>(cp->wscale-1, 1);
			w->write<int>(cp->hscale-1, 1);
			w->write<int>(cp->mk_y, 4);
			w->write<int>(cp->mk_u, 4);
			w->write<int>(cp->mk_v, 4);
			w->write<int>(cp->predictorID-1, 3);
			w->write<int>(cp->N_k, 32);
			w->write<int>(cp->mk_table, 8);
			w->write<int>(cp->WL_max, 32);
			w->write<long>((long)(cp->kmeansTableBitrate*TRUNCATED_DEC), 64);
			w->write<long>((long)(cp->PSNR*TRUNCATED_DEC), 64);
			w->write<long>((long)(cp->adjustBitrate*TRUNCATED_DEC), 64);
			w->write<int>(cp->isWithAdjust, 1);
			w->write<int>(cp->Q_res, 16);
		}
		w->close();
	}
}

void PerformancePackage::load(string name) {
	PerformancePackage* pkg = getInstance(name);
	BitReader* r = BitReader::open(pkg->buildPath());
	if (r->remains() <= 0) { remove(pkg->buildPath().c_str()); return; }
	int parSize = r->read<int>(16);
	CompressionParameters* cp = CompressionParameters::Builder();
	for (int i = 0; i < parSize; i++) {
		cp->imageArea = r->read<int>(32);
		cp->wscale = r->read<int>(1) + 1;
		cp->hscale = r->read<int>(1) + 1;
		cp->mk_y = r->read<int>(4);
		cp->mk_u = r->read<int>(4);
		cp->mk_v = r->read<int>(4);
		cp->predictorID = r->read<int>(3) + 1;
		cp->N_k = r->read<int>(32);
		cp->mk_table = r->read<int>(8);
		cp->WL_max = r->read<int>(32);
		cp->kmeansTableBitrate = (double)(r->read<long>(64)) / TRUNCATED_DEC;
		cp->PSNR = (double)(r->read<long>(64)) / TRUNCATED_DEC;
		cp->adjustBitrate = (double)(r->read<long>(64)) / TRUNCATED_DEC;
		cp->isWithAdjust = r->read<int>(1);
		cp->Q_res = r->read<int>(16);
		pkg->parameters.push_back(cp->clone());
	}
	r->close();
}

string PerformancePackage::buildPath() {
	stringstream ss; ss << CACHE_ROOT << name << CACHE_POSTFIX;
	return ss.str();
}

#endif


	// if (minSearchIdx == maxSearchIdx && par->bitsCount() < this->parameters[minSearchIdx]->bitsCount()) { this->parameters[minSearchIdx] = par; }
	// else if (minSearchIdx < maxSearchIdx) { 
	// 	if (this->parameters[maxSearchIdx]->PSNR > par->PSNR) this->parameters.insert(this->parameters.begin()+maxSearchIdx+1, par->clone());
	// 	else if (this->parameters[minSearchIdx]->PSNR < par->PSNR) this->parameters.insert(this->parameters.begin()+minSearchIdx, par->clone());
	// 	else this->parameters.insert(this->parameters.begin()+maxSearchIdx, par->clone()); 
	// }
	// else if (this->parameters[minSearchIdx]->PSNR > par->PSNR) { this->parameters.insert(this->parameters.begin()+minSearchIdx+1, par->clone()); }
	// else { this->parameters.insert(this->parameters.begin()+minSearchIdx, par->clone()); }
