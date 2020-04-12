
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <chrono>

#define TILE_WIDTH 32

typedef int dato;

using namespace std;
using namespace std::chrono;

template<class T>
void printMatrix(T *M, int rows, int cols) {
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			cout << M[i*cols + j] << '\t';
		}
		cout << endl;
	}
	cout << endl;
}

__global__
void matrixMulKernel(dato *M, dato *N, dato *P, int a, int b, int c) {
	int col = blockIdx.y*blockDim.y + threadIdx.y;
	int row = blockIdx.x*blockDim.x + threadIdx.x;
	if (row < a && col < c) {
		dato Pvalue = 0;
		for (int k = 0; k < b; ++k) {
			Pvalue += M[row*b + k] * N[k*c + col];
		}
		P[row*c + col] = Pvalue;
	}
}

__global__
void matrixMulKernel2(dato *M, dato *N, dato *P, int a, int b, int c) {
	__shared__ dato Mds[TILE_WIDTH][TILE_WIDTH];
	__shared__ dato Nds[TILE_WIDTH][TILE_WIDTH];

	int bx = blockIdx.x, 
		by = blockIdx.y, 
		tx = threadIdx.x, 
		ty = threadIdx.y;
	int row = by * TILE_WIDTH + ty;
	int col = bx * TILE_WIDTH + tx;
	dato pValue = 0;
	for (int ph = 0; ph < ceil(b / (float)TILE_WIDTH); ++ph) {
		if (row < a && (ph*TILE_WIDTH + tx) < b)
			Mds[ty][tx] = M[row*b + ph * TILE_WIDTH + tx];
		else
			Mds[ty][tx] = 0;
		if (col < c && (ph*TILE_WIDTH + ty) < b)
			Nds[ty][tx] = N[(ph*TILE_WIDTH + ty)*c + col];
		else
			Nds[ty][tx] = 0;
		__syncthreads();

		for (int k = 0; k < TILE_WIDTH; ++k) {
			pValue += Mds[ty][k] * Nds[k][tx];
		}
		__syncthreads();
	}
	if(row<a && col<c)
		P[row*c + col] = pValue;
}

__global__
void matrixMulKernel3(dato *M, dato *N, dato *P, int a, int b, int c) {
	__shared__ dato Mds[TILE_WIDTH][TILE_WIDTH];
	__shared__ dato Nds[TILE_WIDTH][TILE_WIDTH];

	int bx = blockIdx.x,
		by = blockIdx.y,
		tx = threadIdx.x,
		ty = threadIdx.y;
	int row = by * TILE_WIDTH + ty;
	int col = bx * TILE_WIDTH + tx;
	dato pValue = 0;
	for (int ph = 0; ph < ceil(b / (float)TILE_WIDTH); ph+=2) {
		if (row < a && (ph*TILE_WIDTH + tx) < b)
			Mds[ty][tx] = M[row*b + ph * TILE_WIDTH + tx];
		else
			Mds[ty][tx] = 0;
		if (col < c && (ph*TILE_WIDTH + ty) < b)
			Nds[ty][tx] = N[(ph*TILE_WIDTH + ty)*c + col];
		else
			Nds[ty][tx] = 0;
		__syncthreads();

		for (int k = 0; k < TILE_WIDTH; ++k) {
			pValue += Mds[ty][k] * Nds[k][tx];
		}
		__syncthreads();
	}
	if (row < a && col < c)
		P[row*c + col] = pValue;
}

void matrixMul(dato *M, dato *N, dato *P, int a, int b, int c, int mode) {
	dato *D_M, *D_N, *D_P;
	int matrixSize1 = a * b;
	int matrixSize2 = b * c;
	int matrixSize3 = a * c;
	cudaMalloc((void**)&D_M, matrixSize1 * sizeof(dato));
	cudaMemcpy(D_M, M, matrixSize1 * sizeof(dato), cudaMemcpyHostToDevice);
	cudaMalloc((void**)&D_N, matrixSize2 * sizeof(dato));
	cudaMemcpy(D_N, N, matrixSize2 * sizeof(dato), cudaMemcpyHostToDevice);
	cudaMalloc((void**)&D_P, matrixSize3 * sizeof(dato));
	auto start = high_resolution_clock::now();
	switch (mode) {
	case 0:
		matrixMulKernel << <dim3(ceil(a / 32.0), ceil(c / 32.0), 1), dim3(32, 32, 1) >> > (D_M, D_N, D_P, a, b, c);
		break;
	case 1:
		matrixMulKernel2 << <dim3(ceil(a / 32.0), ceil(c / 32.0), 1), dim3(32, 32, 1) >> > (D_M, D_N, D_P, a, b, c);
		break;
	case 2:
		matrixMulKernel3 << <dim3(ceil(a / 32.0), ceil(c / 32.0), 1), dim3(32, 32, 1) >> > (D_M, D_N, D_P, a, b, c);
	}
	auto end = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(end - start);
	cout <<"Tiempo en microsegundos: "<< duration.count() << endl;

	cudaMemcpy(P,D_P,matrixSize3*sizeof(dato),cudaMemcpyDeviceToHost);

	cudaFree(D_M);
	cudaFree(D_N);
	cudaFree(D_P);
}

int main(){
	//srand(time(NULL));
	dato *M1, *M2, *M3;
	int a=1000, b=1000, c=1000;
	M1 = new dato[a*b];
	M2 = new dato[b*c];
	M3 = new dato[a*c];
	for (int i = 0, top = a * b; i < top; ++i) {
		M1[i] = rand()%5;
	}
	for (int i = 0, top = b * c; i < top; ++i) {
		M2[i] = rand()%5;
	}
	//printMatrix(M1, a, b);
	//printMatrix(M2, b, c);
	matrixMul(M1, M2, M3, a, b, c, 2);
	printMatrix(M3, a, c);
	//matrixMul(M1, M2, M3, a, b, c, 1);
	//printMatrix(M3, a, c);
	//matrixMul(M1, M2, M3, a, b, c, 0);
    return 0;
}