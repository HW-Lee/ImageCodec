//
//  Transform.h
//  VSP_FINAL_PROJ: IMAGE CODING CONTEST
//
//  Created by HW Lee on 6/19/15.
//  Copyright (c) 2015 HW Lee. All rights reserved.
//

#ifndef TRANSFORM
#define TRANSFORM

#include <iostream>
#include <math.h>

using namespace std;
class Transform {
public:
	template <class T>
	static void dct4(T* v, int size);
	template <class T>
	static void dct4(T* v);

	template <class T>
	static void idct4(T* v, int size);
	template <class T>
	static void idct4(T* v);

};

template <class T>
void Transform::dct4(T* v) { dct4<T>(v, 4); }
template <class T>
void Transform::idct4(T* v) { idct4<T>(v, 4); }

template <class T>
void Transform::dct4(T* v, int size) {
	T temp[4];
	for (int idx = 0; idx < size/4; idx++) {
		temp[0] = v[4*idx+0] + v[4*idx+3];
		temp[1] = v[4*idx+1] + v[4*idx+2];
		temp[2] = v[4*idx+1] - v[4*idx+2];
		temp[3] = v[4*idx+0] - v[4*idx+3];
		v[4*idx+0] = temp[0] + temp[1];
		v[4*idx+2] = temp[0] - temp[1];
		v[4*idx+1] = temp[2] + 2*temp[3];
		v[4*idx+3] = temp[3] - 2*temp[2];
	}
}

template <class T>
void Transform::idct4(T* v, int size) {
	T temp[4];
	for (int idx = 0; idx < size/4; idx++) {
		temp[0] = 5*v[4*idx+0] + 4*v[4*idx+1] + 5*v[4*idx+2] + 2*v[4*idx+3];
		temp[1] = 5*v[4*idx+0] + 2*v[4*idx+1] - 5*v[4*idx+2] - 4*v[4*idx+3];
		temp[2] = 5*v[4*idx+0] - 2*v[4*idx+1] - 5*v[4*idx+2] + 4*v[4*idx+3];
		temp[3] = 5*v[4*idx+0] - 4*v[4*idx+1] + 5*v[4*idx+2] - 2*v[4*idx+3];
		v[4*idx+0] = temp[0]/20;
		v[4*idx+1] = temp[1]/20;
		v[4*idx+2] = temp[2]/20;
		v[4*idx+3] = temp[3]/20;
	}
}

#endif
