#include <stdio.h>
#include <cuda.h>
#include <math.h>
#include <iostream>

// cuda: usr local cuda bin nvcc

#define vecSize 10000

using namespace std;

__global__
void vecAddKernel(float *A, float *B, float *C, int n){
    int i= blockDim.x * blockIdx.x+ threadIdx.x;
    if(i<n) C[i] = A[i] + B[i];
}

void vecAdd(float* h_A, float* h_B, float* h_C, int n){
    int size = n * sizeof(float);
    float *d_A, *d_B, *d_C; 

    cudaMalloc((void **) &d_A, size);
    cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
    cudaMalloc((void **) &d_B, size);
    cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);
    cudaMalloc((void **) &d_C, size);

    vecAddKernel<<<ceil(n/256.0),256>>>(d_A,d_B,d_C,n);

    cudaMemcpy(h_C,d_C,size, cudaMemcpyDeviceToHost);

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
}

int main(){
    float a[vecSize], b[vecSize],c[vecSize];
    for(int i=0; i<vecSize; ++i){
        a[i]=i*2;
        b[i]=i*3;
    }
    vecAdd(a,b,c,vecSize);
    for(int i=0; i<vecSize; ++i)
        cout<<c[i]<<" ";
    cout<<endl;
    int dev_count;
    cudaGetDeviceCount(&dev_count);
    cout<<dev_count<<endl;
    return 0;
}