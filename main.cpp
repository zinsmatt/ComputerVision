#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <list>
#include <algorithm>
#include "images.h"
#include "utils.h"
#include "webcam.h"
#include "hough.h"


using namespace std;
using namespace cv;


int main(int argc, char* argv[])
{
	const char* name1 = "square.jpg";
	const char* name2 = "linkoping.jpg";
	const char* name3 = "building.jpg";
	const char* name4 = "sudoku.jpg";
	const char* name5 = "boules.jpg";
	const char* name6 = "coins.jpg";

	Mat img = imread(name2, CV_LOAD_IMAGE_UNCHANGED);
	if (img.empty())
	{
		std::cerr << "Probleme chargement image\n";
		return 0;
	}

	Mat img_color;
	img.copyTo(img_color);
	try
	{
		cvtColor(img, img, CV_BGR2GRAY);
	}catch(Exception e){
		cout << "image already in gray\n";
	}


	//harrisDetect(img, img_color);
	//edges(img,50);
	//houghLines(img);
	//webcam_edges();
	myHoughLines(img);
	//webcam_corners();
	//houghCircles(img);

	destroyAllWindows();

	return EXIT_SUCCESS;
}



