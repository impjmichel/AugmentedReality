
#include <opencv\cv.h>
#include <opencv\highgui.h>

using namespace cv;

Mat image;

int main() {

	//VideoCapture cap;
	VideoCapture cap;
	cap.open(1);

	// create window to show image
	namedWindow("window", WINDOW_AUTOSIZE);
	moveWindow("window", 400, 100);
	while (1) {

		// cop webcam stream to image
		cap >> image;

		// print image to screen
		imshow("window", image);

		// delay 33 ms
		waitKey(33);
	}
	return 0;
}