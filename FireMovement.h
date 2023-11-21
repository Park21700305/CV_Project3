#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
class FireMovement
{
private:
	Ptr<BackgroundSubtractor> mog2;
	Mat kernel;
	
	vector<vector<Point>> contours;
	vector<Vec4i> hireachy;
public:
	FireMovement();
	~FireMovement();

	int errorSize = 10;

	Mat checkRGB(Mat &original);
	Mat checkYCrCb(Mat &original);
	void drawContours(Mat &orginal, Mat input);
};

