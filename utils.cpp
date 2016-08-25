#include "utils.h"
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>


using namespace std;
using namespace cv;

void print_img(Mat& im)
{
	for (int j = 0; j < im.size().height; j++)
	{
		for(int i = 0; i < im.size().width; i++)
		{
			std::cout << (int)im.at<uchar>(j,i) << " ";
		}
		std::cout << std::endl;
	}
}

void save_img(Mat& img,const char* name)
{
	vector<int> compress_params;
	compress_params.push_back(IMWRITE_JPEG_QUALITY);
	compress_params.push_back(98);
	imwrite(name, img, compress_params);
}
