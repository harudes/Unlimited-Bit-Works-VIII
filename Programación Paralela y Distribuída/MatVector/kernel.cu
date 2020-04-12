
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <iostream>
#include <chrono>

using namespace std;

__global__
void matVecKernel(float* mat, float* vec, float* result, int m) {
	int row = threadIdx.x + blockIdx.x * blockDim.x;
	if (row < m) {
		for (int i = 0; i < m; ++i) {
			result[row] += mat[row*m + i]*vec[i];
		}
	}
}

void matVec(float* mat, float* vec, float* result, int m) {
	float *D_mat, *D_vec, *D_result;
	cudaMalloc((void**)&D_mat,m*m *sizeof(float));
	cudaMemcpy(D_mat,mat,m*m * sizeof(float),cudaMemcpyHostToDevice);
	cudaMalloc((void**)&D_vec, m * sizeof(float));
	cudaMemcpy(D_vec, vec, m * sizeof(float), cudaMemcpyHostToDevice);
	cudaMalloc((void**)&D_result, m * sizeof(float));

	matVecKernel <<< ceil(m / 1024.0), 1024 >>> (D_mat, D_vec, D_result, m);

	cudaMemcpy(result, D_result, m * sizeof(float), cudaMemcpyDeviceToHost);

	cudaFree(D_mat);
	cudaFree(D_vec);
	cudaFree(D_result);
}

int main(){
	int m;
	m = 1000;
	float *mat = new float[m*m], *vec = new float[m], *result = new float[m];
	for (int i = 0; i < m*m; ++i)
		mat[i] = i;
	for (int i = 0; i < m; ++i) {
		vec[i] = 2;
		result[i] = 0;
	}
	matVec(mat,vec,result,m);
	/*for (int i = 0; i < m; ++i)
		cout << result[i] << "\t";
	cout << endl;*/
    return 0;
}
