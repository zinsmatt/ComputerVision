#include "cartoon.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void mainCartoon()
{
	VideoCapture vid = VideoCapture(0);
	if(!vid.isOpened())
	{
		std::cerr << "Cannot open the stream" << endl;
		return;

	}
	vid.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	vid.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	namedWindow("Cartoon",CV_WINDOW_AUTOSIZE);

	Mat frame;
	bool running = true;
	while(running)
	{
		vid >> frame;
		Mat outFrame = Mat::zeros(frame.size(),CV_8UC3);

		cartoonifyImage(frame,outFrame);
		//evilMode(frame,outFrame);
		imshow("Cartoon", outFrame);
		char code = waitKey(20);
		if(code == 27)
		{
			running = false;
		}
	}
}


void cartoonifyImage(Mat& frame, Mat& outFrame)
{
	// create edges mask
	Mat edges, blurred, gray, mask;
	cvtColor(frame, gray, CV_RGB2GRAY);
	medianBlur(gray,blurred,7);
	Laplacian(blurred,edges,CV_8UC1,5);
	threshold(edges,mask,70,255,THRESH_BINARY);


	// create small image
	Size size = frame.size();
	Size smallSize;
	smallSize.width = size.width / 4;
	smallSize.height = size.height / 4;
	Mat smallImg = Mat(smallSize, CV_8UC3);
	resize(frame,smallImg,smallSize,0,0,INTER_LINEAR);

	// apply bilateral filter
	Mat tmp = Mat(smallSize,CV_8UC3);
	int repetitions = 3;
	int ksize = 9;
	double sigmaColor = 13, sigmaSpace = 5;
	for(int repet = 0; repet<repetitions; ++repet)
	{
		bilateralFilter(smallImg,tmp,ksize,sigmaColor, sigmaSpace);
		smallImg = tmp.clone();
	}
	Mat bigImg;
	resize(smallImg,outFrame,size, 0, 0, INTER_LINEAR);
	outFrame.setTo(0, mask);

	/*Mat maskEvil;
	evilMode(frame,maskEvil);
	outFrame.setTo(0, maskEvil);*/
}

void evilMode(Mat& frame, Mat& outFrame)
{
	Mat gray;
	cvtColor(frame,gray,CV_RGB2GRAY);
	medianBlur(gray,gray,7);
	Mat edges, edges2;
	Scharr(gray,edges,CV_8U,1, 0);
	Scharr(gray,edges2,CV_8U,1, 0, -1);
	edges += edges2;
	threshold(edges, outFrame, 12, 255, THRESH_BINARY);
	medianBlur(outFrame, outFrame, 3);
}
