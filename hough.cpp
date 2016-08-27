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
	/* implementation of the hough method to detect lines*/
	Mat im_coul;
	cvtColor(im,im_coul,CV_GRAY2RGB);

	int height_d = ((im.rows>im.cols) ? im.rows : im.cols) * 2 * 1.5;
	int offset = round(height_d / 2);
	Mat acc = Mat::zeros(height_d, 360, CV_16UC1);
	Mat edges, accu_reduced = Mat::zeros(200,360,CV_16UC1);

	GaussianBlur(im,im,Size(3,3),3);
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

void houghCircles(Mat& img)
{
	/* OpenCV function to detect circles */
	Mat img_coul, blurred, edges;
	cvtColor(img,img_coul,CV_GRAY2RGB);

	int threshold1 = 100, threshold2 = 200;
	GaussianBlur(img,blurred,Size(3,3),0);

	vector<Vec3f> circles;
	HoughCircles(blurred,circles,CV_HOUGH_GRADIENT,1,img.rows/12,200,55,0,0);
	std::cout << "size = " << circles.size() << std::endl;
	for(int i=0;i<circles.size();i++)
	{
		Point center(cvRound(circles[i][0]),cvRound(circles[i][1]));
		std::cout << "circle : " << center.x << " " << center.y << std::endl;
		int radius = cvRound(circles[i][2]);
		std::cout <<"radius : " << radius << std::endl;
		circle(img_coul,center,radius,Scalar(0,255,0),3);
	}

	namedWindow("Circles",CV_WINDOW_AUTOSIZE);
	imshow("Circles",img_coul);
	waitKey(0);
}

void voteInAccu(Mat& accu, int pt_x, int pt_y, float dx, float dy)
{
	//add a vote in the accu (dx,dy) should be normalized
	float x = pt_x, y = pt_y;
	accu.at<int>(pt_y,pt_x)--;
	while(round(x) < accu.rows && round(y) < accu.cols && round(x) > 0 && round(y) > 0)
	{
		accu.at<int>(round(y),round(x))++;
		y += dy;
		x += dx;
	}

	x = pt_x; y = pt_y;
	while(round(x) < accu.rows && round(y) < accu.cols && round(x) > 0 && round(y) > 0)
	{
		accu.at<int>(cvRound(y),cvRound(x))++;
		y -= dy;
		x -= dx;
	}
}

void myHoughCircles(Mat& img)
{
	/* implementation of the hough method to detect circles */
	int minDistBetweenCenters = 30;

	Mat img_coul;
	cvtColor(img,img_coul,CV_GRAY2RGB);

	Mat blurred = img.clone();
	GaussianBlur(img,blurred,Size(3,3),3);

	Mat deriv_x, deriv_y;
	Sobel(blurred,deriv_x, CV_16S,1,0);
	Sobel(blurred,deriv_y, CV_16S,0,1);

	Mat edges;
	Canny(blurred,edges,150,200);

	namedWindow("Edges",CV_WINDOW_AUTOSIZE);
	imshow("fen",edges);

	// Find centers
	Mat accu = Mat::zeros(img.rows,img.cols,CV_32SC1);
	int nb_edges = 0;
	bool stop = false;
	for(int j=0; j<edges.rows; j++)
	{
		for(int i=0; i<edges.cols; i++)
		{
			if(edges.at<uchar>(j,i) == 255)
			{// it is an edge
				++nb_edges;
				short dx = deriv_x.at<short>(j,i);
				short dy = deriv_y.at<short>(j,i);
				float norme = sqrt(dx*dx + dy*dy);
				voteInAccu(accu,i,j,(float)dx/norme,(float)dy/norme);
			}
		}
		if(stop) break;
	}
	Mat accu_norm, accu_scaled;
	normalize(accu, accu_norm, 0, 255, NORM_MINMAX, CV_32SC1);
	convertScaleAbs(accu_norm,accu_scaled);

	GaussianBlur(accu_scaled,accu_scaled,Size(5,5),5);

	namedWindow("Hough Space",CV_WINDOW_AUTOSIZE);
	imshow("Hough Space",accu_scaled);

	std::vector<uchar> accu_values;
	for(int j=0; j<accu.rows; ++j)
	{
		for(int i=0; i<accu.rows; ++i)
		{
			accu_values.push_back(accu_scaled.at<uchar>(j,i));
		}
	}

	std::sort(accu_values.begin(), accu_values.end(), descendSorting);
	uchar limite = accu_values[20];

	std::vector<Point> centers;
	for(int j=0; j < img.rows; ++j)
	{
		for(int i=0; i < img.cols; ++i)
		{
			if(accu_scaled.at<uchar>(j,i) >= limite)
			{
				bool ignore = false;
				for(int c=0;c < centers.size(); ++c)
				{
					if(distance(i,j,centers[c].x,centers[c].y)<minDistBetweenCenters)
					{
						ignore = true;
						break;
					}
				}
				if(!ignore)
					centers.push_back(std::move(Point(i,j)));
			}
		}
	}
	std::cout << "nb centers found : " << centers.size() << std::endl;


	// Find radius
	Mat accu_radius = Mat::zeros(centers.size(),round(1.5*max(img.rows,img.cols)), CV_32SC1);
	for(int j=0; j<edges.rows; ++j)
	{
		for(int i=0; i<edges.cols; ++i)
		{
			if(edges.at<uchar>(j,i) == 255)
			{
				for(int k=0; k<centers.size();++k)
				{
					++accu_radius.at<int>(k,distance(i,j,centers[k].x,centers[k].y));
				}
			}
		}
	}

	vector<int> radius;
	for(int k=0; k<centers.size(); ++k)
	{
		int i_max = 0;
		int val_max = 0;
		for(int  i=0; i<accu_radius.cols; ++i)
		{
			if(accu_radius.at<int>(k,i)>val_max)
			{
				val_max = accu_radius.at<int>(k,i);
				i_max = i;
			}
		}
		radius.push_back(i_max);
	}

	for(int i=0; i<radius.size(); ++i)
	{
		circle(img_coul,centers[i],radius[i],Scalar(0,255,255),3);
	}

	namedWindow("Circles",CV_WINDOW_AUTOSIZE);
	imshow("Circles",img_coul);
	waitKey(0);
}
