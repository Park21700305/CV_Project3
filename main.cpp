#include <opencv2/opencv.hpp>
#include <iostream>
#define CVUI_IMPLEMENTATION
#include "cvui.h"
#include "FireMovement.h"
// source: 
//			Advanced Concepts for Intelligent Vision Systems: 11th International - https://books.google.pl/books?id=vUFuCQAAQBAJ&printsec=frontcover&hl=pl#v=onepage&q&f=false
// g++ $(pkg-config --cflags --libs opencv) main.cpp FireMovement.cpp -o test -std=c++11 -Wno-deprecated-declarations
using namespace std;
using namespace cv;
using namespace cvui;

int main(void)
{
	namedWindow("Options");
	init("Options");
	VideoCapture cap;
	cap.open("project3_1.mp4");
	Mat frame, computerVision;
	Mat options(330, 340, CV_8UC3, Scalar(0, 0, 0));
	const char *theLabelFormat = "%.1Lf";
	int segments = 1;
	text(options, 1, 1, "Video samples:");
	text(options, 1, 55, "Error area (contours surface area):");
	text(options, 1, 125, "Detection method:");
	int optionPicked = 1;
	FireMovement f1;
	while (cap.read(frame))
	{
		if (optionPicked == 1) {
			computerVision = f1.checkRGB(frame);
		}
		else {
			computerVision = f1.checkYCrCb(frame);
		}
		
trackbar(options, 11, 70, 220, &f1.errorSize, 1, 100, segments, theLabelFormat, cvui::TRACKBAR_HIDE_VALUE_LABEL, segments);
		if (button(options, 1, 21, "Live")) {
			cap.release();
			cap.open(0);
		}
		if (button(options, 31, 21, "V1")) {
			cap.release();
			cap.open("project3_1.mp4");
		}
		if (button(options, 101, 21, "V2")) {
			cap.release();
			cap.open("project3_3.mp4");
		}
		if (button(options, 171, 21, "V3")) {
			cap.release();
			cap.open("project3_5.mp4");
		}

		if (button(options, 11, 141, "RGB Detect")) {
			optionPicked = 1;
		}
		if (button(options, 111, 141, "YCbCr Detect")) {
			optionPicked = 2;
		}

		f1.drawContours(frame, computerVision);

		update();
		cv::imshow("BackgroundSubtractorMOG2", computerVision);
		cv::imshow("감지영상", frame); 
		cv::imshow("옵션창", options);

		if (waitKey(30) == 27) {
			cap.release();
			destroyAllWindows();
			return 0;
		}
	}
	cap.release();
	waitKey(0);

}