#include "pch.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

int main()
{
	Mat img = imread("descarga.jpg", IMREAD_COLOR);
	namedWindow("image", WINDOW_NORMAL);
	imshow("image", img);
	//cout << 1 << endl;
	int imgSize = img.total()*img.channels();
	//cout << 2 << endl;
	uchar *Pin = img.isContinuous()?img.data:img.clone().data;
	//cout << 3 << endl;
	int newImgSize = img.total();
	//cout << 4 << endl;
	uchar *Pout = new uchar[newImgSize];
	//cout << 5 << endl;
	for (int i = 0; i < newImgSize; ++i) {
		Pout[i] = 0.21* Pin[3 * i] + 0.71*Pin[3*i+1] + 0.07*Pin[3*i+2];
	}

	cout << 6 << endl;

	Mat newImg(img.rows,img.cols, CV_8UC1,Pout,Mat::AUTO_STEP);

	namedWindow("image2", WINDOW_NORMAL);
	imshow("image2", newImg);

	waitKey(0);
	return 0;
}