// ArUcoExample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
using namespace cv;
using namespace std;

Mat img;

int main()
{	
	VideoCapture cap;
	cap.open(1);

	while (1) {
		cap >> img;
		flip(img, img, 1);

		imshow("Normal img", img);

		waitKey(33);
	}

	return 0;
}
