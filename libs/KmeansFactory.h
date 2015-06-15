//
//  KmeansFactory.h
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Created by HW Lee on 6/3/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#ifndef KMEANSFACTORY
#define KMEANSFACTORY

#include <iostream>
#include <math.h>

template <class T, class symbT>
class KmeansInfo {
public:
	~KmeansInfo();
	T* centers;
	symbT* dataSymbols;
	int nCenters;
	int nData;
};

template <class T>
class KmeansFactory{
public:
	~KmeansFactory();
	static KmeansFactory<T>* getInstance();

	KmeansFactory<T>* withRawData(T* data, int dataSize);
	KmeansFactory<T>* clusterInto(int nClusters);

	template <class symbT>
	KmeansInfo<T, symbT>* run(double terminatedRate);
	template <class symbT>
	KmeansInfo<T, symbT>* run();

	void setMaxIteration(int maxIters);

private:
	KmeansFactory();
	void release();

	T* rawData;
	int rawDataSize;
	int nClusters;
	int maxIters;

	static KmeansFactory<T>* instance;

	static const double DEFAULT_TERMINATED_RATE;
};

template <class T>
const double KmeansFactory<T>::DEFAULT_TERMINATED_RATE = 0.005;

template <class T>
KmeansFactory<T>* KmeansFactory<T>::instance = 0;
template <class T>
KmeansFactory<T>* KmeansFactory<T>::getInstance() { if (instance == 0) instance = new KmeansFactory(); return instance; }
template <class T>
KmeansFactory<T>* KmeansFactory<T>::clusterInto(int nClusters) { this->nClusters = nClusters; return this; }
template <class T>
KmeansFactory<T>* KmeansFactory<T>::withRawData(T* data, int dataSize) { this->rawData = data; this->rawDataSize = dataSize; return this; }
template <class T> template <class symbT>
KmeansInfo<T, symbT>* KmeansFactory<T>::run() { return this->run<symbT>(DEFAULT_TERMINATED_RATE); }

template <class T> template <class symbT>
KmeansInfo<T, symbT>* KmeansFactory<T>::run(double terminatedRate) {
	if (this->rawDataSize <= 0) return 0;
	long truncator = 0;
	truncator = ~truncator;
	for (int i = 0; i < sizeof(T); i++) truncator <<= 8;
	truncator = ~truncator;

	double min = (double)(this->rawData[0] & truncator);
	double max = min;

	// find data range
	for (int i = 1; i < this->rawDataSize; i++) {
		double v = (double)(this->rawData[i] & truncator);
		if (max < v) max = v; if (min > v) min = v;
	}

	if (this->nClusters > (max-min+1)) { this->nClusters = truncator; cout << "warning: the # of clusters is too large." << endl; }

	double* centersD = new double[this->nClusters];
	KmeansInfo<T, symbT>* info = new KmeansInfo<T, symbT>();
	info->centers = new T[this->nClusters];
	info->dataSymbols = new symbT[this->rawDataSize];
	info->nCenters = this->nClusters;
	info->nData = this->rawDataSize;

	// init centers
	if (this->nClusters != truncator) {
		double step = (max - min) / this->nClusters;
		for (int i = 0; i < this->nClusters; i++) {
			centersD[i] = min + ((double)i + 0.5)*step; info->centers[i] = round(centersD[i]); 
			if (i > 0 && info->centers[i] == info->centers[i-1]) info->centers[i]++;
		}
	} else { 
		for (int i = 0; i < this->nClusters; i++) info->centers[i] = (T)i;
		for (int i = 0; i < info->nData; i++) info->dataSymbols[i] = 0 + this->rawData[i];
		return info;
	}

	double errRate;
	long searchMin, searchMax;
	double* sumClusters = new double[this->nClusters];
	int* cntClusters = new int[this->nClusters];
	int nIter = 0;
	while(nIter < this->maxIters) {
		errRate = 0;
		for (int i = 0; i < this->nClusters; i++) { sumClusters[i] = 0; cntClusters[i] = 0; }
		for (int i = 0; i < this->rawDataSize; i++) {
			searchMin = 0; searchMax = this->nClusters-1;
			while (searchMax - searchMin > 1) {
				long curIdx = floor( (float)(searchMax+searchMin)/2 );
				if ( (double)(info->centers[curIdx] & truncator) >= (double)(this->rawData[i] & truncator) ) { searchMax = curIdx; }
				if ( (double)(info->centers[curIdx] & truncator) <= (double)(this->rawData[i] & truncator) ) { searchMin = curIdx; }
			}
			if ( ( (double)(info->centers[searchMax] & truncator)-(double)(this->rawData[i] & truncator) ) > 
					( (double)(this->rawData[i] & truncator)-(double)(info->centers[searchMin] & truncator) ) ) {
				info->dataSymbols[i] = searchMin; sumClusters[searchMin] += (double)(this->rawData[i] & truncator); cntClusters[searchMin]++;
			} else {
				info->dataSymbols[i] = searchMax; sumClusters[searchMax] += (double)(this->rawData[i] & truncator); cntClusters[searchMax]++;
			}
		}
		for (int i = 0; i < this->nClusters; i++) {
			if (cntClusters[i] > 0) errRate += fabs((sumClusters[i] / (double)cntClusters[i]) - centersD[i]) / (centersD[i]+1);
			if (cntClusters[i] > 0) centersD[i] = sumClusters[i] / (double)cntClusters[i];
			else centersD[i] = (double)(info->centers[i] & truncator);
			info->centers[i] = round(centersD[i]);
		}

		// check for ascending property to check if sorting is needed. (generally no need to sort)
		for (int i = 1; i < this->nClusters; i++) {
			if ((double)(info->centers[i] & truncator) < (double)(info->centers[i-1] & truncator)) {
				// cout << "OMG, sort again: " << (double)(info->centers[i] & truncator) << " < " << (double)(info->centers[i-1] & truncator) << endl;
				// sort
				for (int j = 0; j < this->nClusters-1; j++) {
					for (int k = j; k < this->nClusters; k++) {
						if ( (double)(info->centers[j] & truncator) > (double)(info->centers[k] & truncator) ) {
							T temp = info->centers[k]; info->centers[k] = info->centers[j]; info->centers[j] = temp;
						}
					}
				}
				break;
			}
		}
		if ( errRate/this->nClusters < terminatedRate ) break;
		nIter++;
	}
	// cout << "terminate after " << nIter << " iterations." << endl;

	delete[] centersD; delete[] sumClusters; delete[] cntClusters;
	return info;
}

template <class T>
void KmeansFactory<T>::setMaxIteration(int maxIters) { this->maxIters = maxIters; }

template <class T>
void KmeansFactory<T>::release() { delete[] rawData; }

template <class T>
KmeansFactory<T>::KmeansFactory() { this->maxIters = 50; }

template <class T>
KmeansFactory<T>::~KmeansFactory() {}

template <class T, class symbT>
KmeansInfo<T, symbT>::~KmeansInfo() { delete[] this->centers; delete[] this->dataSymbols; }

#endif
