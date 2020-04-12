
#include <cuda.h>

#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

__global__
void matrixSumKernel1(float *mat1, float *mat2, float *mat3, int m, int n) {
	int col = threadIdx.x + blockIdx.x * blockDim.x;
	int row = threadIdx.y + blockIdx.y * blockDim.y;
	if (col < n && row < m) {
		mat3[row*n + col] = mat1[row*n + col] + mat2[row*n + col];
	}
}

__global__
void matrixSumKernel2(float *mat1, float *mat2, float* mat3, int m, int n) {
	int row = threadIdx.x + blockIdx.x * blockDim.x;
	if (row < m) {
		for (int i = 0; i < n; ++i) {
			mat3[row*n+i] = mat1[row*n + i] + mat2[row*n + i];
		}
	}
}

__global__
void matrixSumKernel3(float *mat1, float *mat2, float* mat3, int m, int n) {
	int col = threadIdx.x + blockIdx.x * blockDim.x;
	if (col < n) {
		for (int i = 0; i < m; ++i) {
			mat3[i*n + col] = mat1[i*n + col] + mat2[i*n + col];
		}
	}
}

void matrixSum(float* mat1, float* mat2, float* mat3, int m, int n, int mode) {
	if (mode <= 2 && mode >= 0) {
		int matSize = m * n;
		float *D_mat1, *D_mat2, *D_mat3;
		cudaMalloc((void **)&D_mat1, matSize*sizeof(float));
		cudaMemcpy(D_mat1, mat1, matSize * sizeof(float), cudaMemcpyHostToDevice);
		cudaMalloc((void **)&D_mat2, matSize * sizeof(float));
		cudaMemcpy(D_mat2, mat2, matSize * sizeof(float), cudaMemcpyHostToDevice);
        cudaMalloc((void **)&D_mat3, matSize * sizeof(float));
        auto start = high_resolution_clock::now();
		switch (mode) {
		case 0:
			matrixSumKernel1 <<< dim3(ceil(n / 32.0), ceil(m / 32.0), 1), dim3(32, 32, 1) >>> (D_mat1, D_mat2, D_mat3, m, n);
			break;
		case 1:
			matrixSumKernel2 <<< ceil(m / 1024.0), 1024 >>> (D_mat1, D_mat2, D_mat3, m, n);
			break;
		case 2:
			matrixSumKernel3 <<< ceil(n / 1024.0), 1024 >>> (D_mat1, D_mat2, D_mat3, m, n);
			break;
		}
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<nanoseconds>(stop - start);
		cout<<"Tiempo kernel: "<< duration.count()<<" nanosegundos"<<endl;
        
		cudaMemcpy(mat3, D_mat3, matSize * sizeof(float), cudaMemcpyDeviceToHost);
		cudaFree(D_mat1);
		cudaFree(D_mat2);
		cudaFree(D_mat3);
	}
}

int main(int argc, char* argv[]){
	cout<<"Suma de matrices"<<endl;
	int m, n;
	m = 1000;
	n = 1500;
	cout<<"Matrices de dimension "<<m<<"x"<<n<<endl;
	float *mat1 = new float[m*n], *mat2 = new float[m*n], *mat3 = new float[m*n];
	for (int i = 0, size = m*n; i < size; ++i) {
		mat1[i] = i;
		mat2[i] = size - i;
	}
	for(int i=0; i<3; ++i){
		auto start = high_resolution_clock::now();
		matrixSum(mat1, mat2, mat3, m, n, i);
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<nanoseconds>(stop - start);
		cout<<"Tiempo total: "<< duration.count()<<" nanosegundos con el metodo "<<i+1<<endl;
	}
	/*for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			cout << mat3[i*n+j] << " ";
		}
		cout << endl;
	}
	cout << endl;*/
    return 0;
}
