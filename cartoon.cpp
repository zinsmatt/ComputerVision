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
		//alienMode(frame,outFrame);
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
	Mat bigImg, tempFrame;
	resize(smallImg,tempFrame,size, 0, 0, INTER_LINEAR);
	tempFrame.setTo(0, mask);

	/*Mat maskEvil;
	evilMode(frame,maskEvil);
	outFrame.setTo(0, maskEvil);*/
	alienMode(tempFrame,outFrame, smallImg, mask);

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

void alienMode(Mat& frame, Mat& outFrame, Mat& smallFrame, Mat& edges)
{
	Scalar color(CV_RGB(255,255,0));
	Size size = frame.size();
	int thickness = 4;
	int sw = size.width;
	int sh = size.height;
	int faceH = sh/2 * 70/100;
	int faceW = faceH * 72/100;
	outFrame = frame.clone();

	// draw head
	ellipse(outFrame, Point(sw/2,sh/2), Size(faceW,faceH),0 ,0, 360, color, thickness, CV_AA);

	// draw eyes
	int eyeW = faceW * 23/100;
	int eyeH = faceH * 11/100;
	int eyeX = faceW * 48/100;
	int eyeY = faceH * 13/100;
	Size eyeSize = Size(eyeW, eyeH);
	int eyeA = 15;
	int eyeYshift = 11;
	ellipse(outFrame, Point(sw/2 - eyeX, sh/2 - eyeY), eyeSize, 0, 180+eyeA, 360-eyeA, color, thickness, CV_AA);
	ellipse(outFrame, Point(sw/2 - eyeX, sh/2 - eyeY - eyeYshift), eyeSize, 0, 0+eyeA, 180-eyeA, color, thickness, CV_AA);
	ellipse(outFrame, Point(sw/2 + eyeX, sh/2 - eyeY), eyeSize, 0, 180+eyeA, 360-eyeA, color, thickness, CV_AA);
	ellipse(outFrame, Point(sw/2 + eyeX, sh/2 - eyeY - eyeYshift), eyeSize, 0, 0+eyeA, 180-eyeA, color, thickness, CV_AA);

	// draw mouth
	int mouthY = faceH * 48/100;
	int mouthW = faceW * 45/100;
	int mouthH = faceH * 6/100;
	ellipse(outFrame, Point(sw/2, sh/2 + mouthY), Size(mouthW, mouthH), 0, 0, 180, color, thickness, CV_AA);

	// draw text
	int fontFace = FONT_HERSHEY_COMPLEX;
	float fontScale = 1.0f;
	int fontThickness = 2;
	char *msg = "Put your face here";
	putText(outFrame, msg, Point(sw * 23/100, sh * 10/100), fontFace, fontScale, color, fontThickness, CV_AA);


	Mat yuv = Mat(smallFrame.size(), CV_8UC3);
	cvtColor(smallFrame, yuv, CV_BGR2YCrCb);

	sw = smallFrame.size().width;
	sh = smallFrame.size().height;
	Mat mask, maskPlusBorder;
	maskPlusBorder = Mat::zeros(sh+2, sw+2, CV_8UC1);
	mask = maskPlusBorder(Rect(1,1,sw,sh));
	resize(edges,mask,smallFrame.size());
	const int EDGES_THRESHOLD = 80;
	threshold(mask, mask, EDGES_THRESHOLD, 255, THRESH_BINARY);
	dilate(mask,mask,Mat());
	erode(mask,mask,Mat());

	int const NUM_SKIN_POINTS = 6;
	Point skinPts[NUM_SKIN_POINTS];
	skinPts[0] = Point(sw/2			, sh/2-sh/6);
	skinPts[1] = Point(sw/2-sw/11	, sh/2-sh/6);
	skinPts[2] = Point(sw/2+sw/11	, sh/2-sh/6);
	skinPts[3] = Point(sw/2			, sh/2+sh/6);
	skinPts[4] = Point(sw/2-sw/9	, sh/2+sh/6);
	skinPts[5] = Point(sw/2+sw/9	, sh/2+sh/6);

	const int LOWER_Y  = 60;
	const int UPPER_Y  = 80;
	const int LOWER_Cr = 15;
	const int UPPER_Cr = 25;
	const int LOWER_Cb = 15;
	const int UPPER_Cb = 20;

	Scalar lowerDiff = Scalar(LOWER_Y, LOWER_Cr, LOWER_Cb);
	Scalar upperDiff = Scalar(UPPER_Y, UPPER_Cr, UPPER_Cb);

	const int CONNECTED_COMPONENTS = 4;
	const int flags = CONNECTED_COMPONENTS | FLOODFILL_FIXED_RANGE | FLOODFILL_MASK_ONLY;
	Mat edgesMask = mask.clone();
	//for(int i=0;i< 1/*NUM_SKIN_POINTS*/; i++)
	//{
	//	std::cout << "iteration 1 \n";
	//	floodFill(yuv, maskPlusBorder, skinPts[i], Scalar(), NULL, lowerDiff, upperDiff, flags);
	//}*/


	//outFrame = maskPlusBorder.clone();
	for(int i=0;i<NUM_SKIN_POINTS;i++)
	{
		circle(mask,skinPts[i],8,Scalar(255,0,0),2);
		floodFill(yuv, maskPlusBorder, skinPts[i], Scalar(), NULL, lowerDiff, upperDiff, flags);

	}
	mask -= edgesMask;
	resize(mask, outFrame, outFrame.size());

	//mask *= 255;
	/*for(int j=0;j<outFrame.rows;j++)
	{
		for(int i=0;i<outFrame.cols;i++)
		{
			if(outFrame.at<uchar>(j,i) == 1)
				outFrame.at<uchar>(j,i) = 255;
		}
	}*/

}
