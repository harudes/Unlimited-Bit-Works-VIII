#include "cuda.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#define CHANNELS 3
#define BLUR_SIZE 6

__global__
void colorToGreyscaleConversion(unsigned char * Pout, unsigned	char * Pin, int width, int height) {
	int Col = threadIdx.x + blockIdx.x * blockDim.x;
	int Row = threadIdx.y + blockIdx.y * blockDim.y;
	if (Col < width && Row < height) {
		int greyOffset = Row * width + Col;
		int rgbOffset = greyOffset * CHANNELS;
		unsigned char r = Pin[rgbOffset];
		unsigned char g = Pin[rgbOffset + 1];
		unsigned char b = Pin[rgbOffset + 2];
		Pout[greyOffset] = 0.21f*r + 0.71f*g + 0.07f*b;
	}
}

void gray(unsigned char *Pin, unsigned char *Pout, int width, int height) {
	uchar *d_pin;
	uchar *d_pout;
	cudaMalloc((void**)&d_pin, width*height * 3);
	cudaMalloc((void**)&d_pout, width*height);

	cudaMemcpy(d_pin, Pin, width*height*3,cudaMemcpyHostToDevice);
	
	colorToGreyscaleConversion <<< dim3(ceil(width / 32.0), ceil(height / 32.0), 1), dim3(32, 32, 1) >>> (d_pout, d_pin, width, height);

	cudaMemcpy(Pout, d_pout, width*height, cudaMemcpyDeviceToHost);

	cudaFree(d_pin);
	cudaFree(d_pout);
}

__global__
void blurKernel(unsigned char *in, unsigned char *out, int w, int h) {
	int Col = blockIdx.x * blockDim.x + threadIdx.x;
	int Row = blockIdx.y * blockDim.y + threadIdx.y;
	if (Col < w && Row < h) {
		for (int i = 0; i < 3; ++i) {
			int pixVal = 0;
			int pixels = 0;
			for (int blurRow = -BLUR_SIZE; blurRow < BLUR_SIZE + 1; ++blurRow) {
				for (int blurCol = -BLUR_SIZE; blurCol < BLUR_SIZE + 1; ++blurCol) {
					int curRow = Row + blurRow;
					int curCol = Col + blurCol;
					if (curRow > -1 && curRow < h && curCol > -1 && curCol < w) {
						pixVal += in[(curRow * w + curCol)*3 + i];
						pixels++;
					}
				}
			}
			out[(Row * w + Col)*3 + i] = (unsigned char)(pixVal / pixels);
		}
	}
}

void blur(unsigned char *in, unsigned char *out, int w, int h) {
	uchar *d_in;
	uchar *d_out;
	cudaMalloc((void**)&d_in, w*h * 3);
	cudaMalloc((void**)&d_out, w*h * 3);

	cudaMemcpy(d_in, in, w*h * 3, cudaMemcpyHostToDevice);

	blurKernel <<< dim3(ceil(w / 32.0), ceil(h / 32.0), 1), dim3(32, 32, 1) >>> (d_in, d_out, w, h);

	cudaMemcpy(out, d_out, w*h*3, cudaMemcpyDeviceToHost);

	cudaFree(d_in);
	cudaFree(d_out);
}

void maingray(string image) {
	Mat img = imread(image, IMREAD_COLOR);
	namedWindow("image");
	imshow("image", img);
	int imgSize = img.total()*img.channels();
	uchar *Pin = img.isContinuous() ? img.data : img.clone().data;
	int newImgSize = img.total();
	uchar *Pout = new uchar[newImgSize];

	gray(Pin, Pout, img.cols, img.rows);

	Mat newImg(img.rows, img.cols, CV_8UC1, Pout, Mat::AUTO_STEP);
	imwrite("gray_" + image, newImg);
	namedWindow("image2");
	imshow("image2", newImg);
	waitKey(0);
}

void mainblur(string image) {
	Mat img = imread(image, IMREAD_COLOR);
	namedWindow("image");
	imshow("image", img);
	int imgSize = img.total()*img.channels();
	uchar *Pin = img.isContinuous() ? img.data : img.clone().data;
	uchar *Pout = new uchar[imgSize];

	blur(Pin, Pout, img.cols, img.rows);

	Mat newImg(img.rows, img.cols, CV_8UC3, Pout, Mat::AUTO_STEP);
	imwrite("blur_" + image, newImg);
	namedWindow("image2");
	imshow("image2", newImg);
	waitKey(0);
}

int main(int argc, char** argv)
{
	//maingray("dmc5.jpg");
	mainblur("dmc5.jpg");
}