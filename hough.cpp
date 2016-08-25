#include "hough.h"
#include "utils.h"
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

bool descendSorting(int i, int j) { return i>j; }

void houghLines(Mat &img)
{
	/* Detect lines with OpenCV functions */
	bool normalHough = false;
	Mat dst, cdst;
	Canny(img, dst, 150, 200, 3);
	cvtColor(dst, cdst, CV_GRAY2BGR);
	vector<Vec2f> lines;
	vector<Vec4i> lines_i;

	if (normalHough)
	{
		std::cout << "Normal Hough Line" << std::endl;
		HoughLines(dst, lines, 1, CV_PI / 180, 100, 0, 0);
		std::cout << "nb lines = " << lines.size() << std::endl;
		for (size_t i = 0; i < lines.size() && i < 50; i++)
		{
			float rho = lines[i][0], theta = lines[i][1];
			Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000 * (-b));
			pt1.y = cvRound(y0 + 1000 * (a));
			pt2.x = cvRound(x0 - 1000 * (-b));
			pt2.y = cvRound(y0 - 1000 * (a));
			line(cdst, pt1, pt2, Scalar(0, 255, 0), 1, CV_AA);
		}
	}
	else
	{
		std::cout << "Probabilistic Hough Lines" << std::endl;
		HoughLinesP(dst, lines_i, 1, CV_PI / 180, 50, 50, 10);
		for (size_t i = 0; i < lines_i.size(); i++)
		{
			Vec4i l = lines_i[i];
			line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 1, CV_AA);
		}
	}
	namedWindow("test", CV_WINDOW_AUTOSIZE);
	imshow("test", cdst);
	waitKey();
}


void myHoughLines(Mat& im)
{
	/* implementation of hough method */
	Mat im_coul;
	cvtColor(im,im_coul,CV_GRAY2RGB);

	int height_d = ((im.rows>im.cols) ? im.rows : im.cols) * 2 * 1.5;
	int offset = round(height_d / 2);
	Mat acc = Mat::zeros(height_d, 360, CV_16UC1);
	Mat edges, accu_reduced = Mat::zeros(200,360,CV_16UC1);

	Canny(im, edges, 150, 200, 3);

	namedWindow("Hough Space",CV_WINDOW_AUTOSIZE);
	namedWindow("Canny",CV_WINDOW_AUTOSIZE);
	namedWindow("Lines",CV_WINDOW_AUTOSIZE);

	int nb = 0;
	// fill the accumulator in hough space
	for(int j=0;j<im.rows;j++)
	{
		for(int i=0;i<im.cols;i++)
		{
			if(edges.at<uchar>(j,i) == 255)
			{
				nb++;
				for(float angle = 0; angle <= 180; angle+=0.5)
				{
					double d = i * cos(TO_RADIAN(angle)) + j * sin(TO_RADIAN(angle));
					++acc.at<uint16_t>(round(d+offset),angle*2);
					++accu_reduced.at<uint16_t>(round(100 + (d*100/offset)),angle*2);
				}
			}
		}
	}

	Mat accu_norm, accu_scaled,	color_map;

	// nomarlize accumlator
	normalize(accu_reduced, accu_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(accu_norm, accu_scaled);
	// apply colormap
	applyColorMap(accu_scaled, color_map, COLORMAP_HOT);

	// display accumulator and edges
	imshow("Hough Space",color_map);
	imshow("Canny",edges);


	// find peaks
	std::vector<int> values;
	for(int j=0;j<acc.rows;j++)
	{
		for(int i=0;i<acc.cols;i++)
		{
			int val = (int) acc.at<uint16_t>(j,i);
			if(val>5)
				values.push_back(val);
		}
	}

	std::sort(values.begin(),values.end(),descendSorting);		// sort values
	int limit = values[50];

	double y1,y0;
	Point a,b;
	float angle;
	int d;

	// draw lines
	for(int j=0;j<acc.rows;j++)
	{
		for(int i=0;i<acc.cols;i++)
		{
			if(acc.at<uint16_t>(j,i)>=limit)
			{
				int x0 = 0, x1 = im.cols-1;
				d = (j - offset);
				angle = i/2;

				if(angle == 0)	// vertical line
				{
					cout << "VERTICAL : d = "<<d<<" angle = 0\n";
					y0 = 0;
					y1 = im.rows-1;
					x0 = d;
					x1 = d;
				}else
				{
					cout << "HORIZONTAL : d =  " << d << " angle = " << angle << "\n";
					y0 = d / sin(TO_RADIAN(angle));
					y1 = (d - x1 * cos(TO_RADIAN(angle))) / sin(TO_RADIAN(angle));
				}
				a.x = round(x0);
				a.y = round(y0);
				b.x = round(x1);
				b.y = round(y1);
				line(im_coul,a,b,Scalar(0,255,0),1);
			}
		}
	}
	imshow("Lines",im_coul);
	waitKey(0);
}


