#include "FireMovement.h"
using namespace std;
using namespace cv;


FireMovement::FireMovement()
{
	mog2 = createBackgroundSubtractorMOG2();
	kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	errorSize = 10;
	
}


FireMovement::~FireMovement()
{

}

Mat FireMovement::checkRGB(Mat &original)
{
	Mat fgmask;
	mog2->apply(original, fgmask, 0.001);//Apply mask to determine moving picture
	Mat dst;
	dst.create(original.size(), CV_8UC1);
	int Rt = 115;//Treshold of red channel (const)
	int St = 45;//Treshold of saturation channel (const)
	Mat RGB[3];
	split(original, RGB);
	for (int i = 0; i < original.rows; i++)//Two for() to check every possible pixel
	{
		for (int j = 0; j < original.cols; j++)
		{
			float B = RGB[0].at<uchar>(i, j);//B - channel B of pixel
			float G = RGB[1].at<uchar>(i, j);//G - channel G of pixel
			float R = RGB[2].at<uchar>(i, j);//R - channel R of pixel
			double Sat = 0;
			int minValue = min(R, min(G, B));
			int maxValue = max(R, max(G, B)) / 255;
			if (maxValue == 0) {
				Sat = 0;
			}
			else {
				Sat = (maxValue - minValue) / maxValue;
			}
			double S = (1 - 3.0 * minValue / (R + G + B));//Saturation of pixel (We need to calculate it)
			if (fgmask.at<uchar>(i, j) > 0 && R > Rt && R >= G && G >= B && S > 0.20 && S > ((255 - R) * St / Rt))//3 conditions from Method: 2 Page: 16 book: (look sources)
			{
				dst.at<uchar>(i, j) = 255;//Coloring pixel which indicates fire
			}
			else
			{
				dst.at<uchar>(i, j) = 0;//Pixel that doesn't indicate fire
			}
		}
	}
	dilate(dst, dst, kernel, Point(-1, -1));//Dilate for future drawing countours near multiple fire sources
	return dst;
}

Mat FireMovement::checkYCrCb(Mat &original)
{
	cv::Vec3f pixelYCbCr, pixelYCbCr1;
	Mat colorMask;
	colorMask.create(original.size(), CV_8UC1);
	Mat fgmask;
	mog2->apply(original, fgmask, 0.001);//Apply mask to determine moving picture
	Mat dst;
	dst.create(original.size(), CV_8UC1);
	Mat RGB[3];
	Scalar meanY;
	Scalar meanCr;
	Scalar meanCb;
	split(original, RGB);
	meanY = mean(RGB[0]);
	meanCb = mean(RGB[1]);
	meanCr = mean(RGB[2]);

	for (int i = 0; i < original.rows; i++)//Two for() to check every possible pixel
	{
		for (int j = 0; j < original.cols; j++)
		{
			pixelYCbCr = original.at<cv::Vec3b>(i, j);
			double Y = 16 + (65.738*pixelYCbCr[2]) / 256 + (129.057*pixelYCbCr[1]) / 256 + (25.064*pixelYCbCr[0]) / 256;
			double Cb = 128 - (37.945*pixelYCbCr[2]) / 256 - (74.494*pixelYCbCr[1]) / 256 + (112.439*pixelYCbCr[0]) / 256;
			double Cr = 128 + (112.439*pixelYCbCr[2]) / 256 - (94.154*pixelYCbCr[1]) / 256 - (18.285*pixelYCbCr[0]) / 256;
			double Ymean = 16 + (65.738*meanCr[0]) / 256 + (129.057*meanCb[0]) / 256 + (25.064*meanY[0]) / 256;
			double Cbmean = 128 - (37.945*meanCr[0]) / 256 - (74.494*meanCb[0]) / 256 + (112.439*meanY[0]) / 256;
			double Crmean = 128 + (112.439*meanCr[0]) / 256 - (94.154*meanCb[0]) / 256 - (18.285*meanY[0]) / 256;
			uchar* colorMaskValuePt = colorMask.ptr<uchar>(i);

			if (fgmask.at<uchar>(i, j) > 0 && (Y > Cb && Cr > Cb &&
				(Y > Ymean && Cb < Cbmean && Cr > Crmean) &&
				((abs(Cb - Cr) * 256) >= 40))) {
				dst.at<uchar>(i, j) = 255;//Coloring pixel which indicates fire
			}
			else
			{
				dst.at<uchar>(i, j) = 0;//Pixel that doesn't indicate fire
			}
		}
	}
	dilate(dst, dst, kernel, Point(-1, -1));//Dilate for future drawing countours near multiple fire sources
	return dst;
}


void FireMovement::drawContours(Mat &orginal, Mat input)
{
	findContours(input, contours, hireachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
	for (int i = 0; i < contours.size(); i++) {
		Rect selection = boundingRect(contours[i]);
		if (selection.width < errorSize || selection.height < errorSize)//for smaller objects dont draw rectangle
			continue;
		rectangle(orginal, selection, Scalar(0, 255, 0), 2, 8, 0);
	}
}

