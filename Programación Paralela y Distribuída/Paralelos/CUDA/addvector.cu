#include <stdio.h>
#include <cuda.h>
#include <math.h>


__global__
void vecAddKernel(float* A, float* B, float* C, int n){
	printf("-A: %f B: %f ",*A,*B);
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if(i<n) C[i] = A[i] + B[i];
	printf("C: %f \n", *C);
}
void vecAdd(float* A, float* B, float* C, int n){
	int size = n * sizeof(float);
	float *d_A, *d_B, *d_C;
	
	cudaMalloc((void**) &d_A,size);
	cudaMemcpy(d_A, A,size,cudaMemcpyHostToDevice);
	cudaMalloc((void**) &d_B,size);
	cudaMemcpy(d_B, B,size,cudaMemcpyHostToDevice);
	
	cudaMalloc((void**) &d_C,size);
	//vecAddKernel<<<ceil(n/256.0),256>>>(d_A,d_B,d_C,n);
	vecAddKernel<<<1,10>>>(d_A,d_B,d_C,n);
	cudaMemcpy(C, d_C,size, cudaMemcpyDeviceToHost);
	cudaFree(d_A);cudaFree(d_B);cudaFree(d_C);
	printf("d_c: %f \n", *C);
}

int main(){
	printf("nani: \n");
	float A[10];
	float B[10];	
	for(int i=0;i<10;++i){
		A[i] = 1.0;
		B[i] = 2.0;
	}
	float C[10];
	vecAdd(A,B,C,10);
	for(int i=0;i<10;++i){
		printf("%f ",C[i]);
	}
	printf("\n");
	return 0;
}

/**usr/local/cuda/bin*/
