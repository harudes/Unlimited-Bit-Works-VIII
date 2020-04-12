#include <cuda.h>
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

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

	auto start = high_resolution_clock::now();
	matVecKernel <<< ceil(m / 1024.0), 1024 >>> (D_mat, D_vec, D_result, m);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<nanoseconds>(stop - start);
	cout<<"Tiempo kernel: "<< duration.count()<<" nanosegundos"<<endl;

	cudaMemcpy(result, D_result, m * sizeof(float), cudaMemcpyDeviceToHost);

	cudaFree(D_mat);
	cudaFree(D_vec);
	cudaFree(D_result);
}

int main(){
	cout<<"Multiplicacion matriz vector"<<endl;
	int m;
	m = 1000;
	cout<<"Matriz de dimension "<<m<<"x"<<m<<endl;
	float *mat = new float[m*m], *vec = new float[m], *result = new float[m];
	for (int i = 0; i < m*m; ++i)
		mat[i] = i;
	for (int i = 0; i < m; ++i) {
		vec[i] = 2;
		result[i] = 0;
	}
	auto start = high_resolution_clock::now();
	matVec(mat,vec,result,m);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<nanoseconds>(stop - start);
	cout<<"Tiempo total: "<< duration.count()<<" nanosegundos"<<endl;
	
	/*for (int i = 0; i < m; ++i)
		cout << result[i] << "\t";
	cout << endl;*/
    return 0;
}
