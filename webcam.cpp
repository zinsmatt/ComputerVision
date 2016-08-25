#include "webcam.h"
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;



void webcam_edges()
{
	/*edges detection on webcam stream*/
	VideoCapture vid = VideoCapture(0);
	if(!vid.isOpened())
	{
		std::cerr << "Cannot open the stream" << endl;
		return;
	}
	namedWindow("MyCam",CV_WINDOW_AUTOSIZE);
	bool recording = true;
	int ratio = 3;
	float lowTreshold = 50;
	int kernelSize = 3;
	while(recording)
	{
		Mat frame,dst;
		bool success = vid.read(frame);
		if(!success) return;

		Canny(frame, dst, lowTreshold, ratio * lowTreshold, kernelSize);
		imshow("MyCam",dst);

		int code = waitKey(30);
		if(code == 1114027)		//'+'
			lowTreshold += 5;
		else if(code == 1114029)	//'-'
			lowTreshold -= 5;
		else if(code == 1113938)	//'arrow up'
			ratio += 0.1;
		else if (code == 1113940)	//'arrow down'
			ratio -= 0.1;
		else if(code == 1048603)	//'escape'
		{
			cout << "esc key is pressed by user" << endl;
			recording = false;
		}
	}
}

void webcam_corners()
{
	VideoCapture vid = VideoCapture(0);
	if(vid.isOpened() == false)
	{
		cerr << "Cannot open webcam" << endl;
		return;
	}

	// parameters
	int blockSize = 2;
	int apertureSize = 3;
	double k = 0.04;
	int limite = 50;

	namedWindow("fen",CV_WINDOW_AUTOSIZE);
	namedWindow("fen2",CV_WINDOW_AUTOSIZE);
	bool recording = true;
	Mat points, points_norm, points_norm_scaled, frame, gray_frame;

	while(recording)
	{
		int nb_points = 0;
		vid.read(frame);
		cvtColor(frame,gray_frame,CV_BGR2GRAY);

		cornerHarris(gray_frame, points, blockSize, apertureSize, k, BORDER_DEFAULT);
		normalize(points, points_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
		convertScaleAbs(points_norm, points_norm_scaled);

		for (int i = 0; i < points_norm.rows; i++)
		{
			for (int j = 0; j < points_norm.cols; j++)
			{
				if ((int)points_norm.at<float>(i, j) >= limite && nb_points<500)
				{
					circle(frame, Point(j, i), 3, Scalar(255,0, 0), 2, 8, 0);
					++nb_points;
				}
			}
		}
		imshow("fen",points_norm_scaled);
		imshow("fen2",frame);
		int ret = waitKey(15);
		if(ret == 1048603)
			recording = false;
	}
}
