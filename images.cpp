#include "images.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

void harrisDetect(Mat& src, Mat& src_color)
{
	Mat dst, dst_norm, dst_norm_scaled;
	dst = Mat::zeros(src.size(), CV_32FC1);

	// parameters
	int blockSize = 2;
	int apertureSize = 3;
	double k = 0.04;
	int limite = 150;

	cornerHarris(src, dst, blockSize, apertureSize, k, BORDER_DEFAULT);
	normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(dst_norm, dst_norm_scaled);

	int nb_corners = 0;
	for (int i = 0; i < dst_norm_scaled.rows; i++) {
		for (int j = 0; j < dst_norm_scaled.cols; j++)
		{
			if ((int)dst_norm.at<float>(i, j) >= limite && nb_corners <= 200)
			{
				circle(src_color, Point(j, i), 2, Scalar(0, 255, 0), 2, 8, 0);
				nb_corners++;
			}
		}
	}
	namedWindow("harris", CV_WINDOW_AUTOSIZE);
	imshow("harris", src_color);
	waitKey();
	destroyWindow("harris");
}

void edges(Mat& img, int low_th)
{
	int ratio = 3, kernel_size = 3;
	Mat smoothed_img, detected_edges;

	blur(img, smoothed_img, Size(3,3));
	Canny(smoothed_img, detected_edges, low_th, low_th * ratio, kernel_size);
	namedWindow("canny", CV_WINDOW_AUTOSIZE);
	imshow("canny", detected_edges);
	waitKey(0);

}

void noise_and_blur(Mat& img)
{
	/* blur image with salt and pepper noise */
	Mat noise = Mat(img.size(), CV_8U);
	Scalar a(0), b(15);
	randn(noise, a, b);
	img = img + noise;

	namedWindow("original", CV_WINDOW_AUTOSIZE);
	imshow("original", img);
	Mat dst;
	char zBuffer[35];
	for(int i = 1; i < 41; i += 2)
	{
		snprintf(zBuffer, 35, "Kernel Size : %d x %d", i, i);
		//blur(img, dst, Size(i, i));
		//GaussianBlur(img, dst, Size(i, i), 0, 0);
		//medianBlur(img, dst, i);
		bilateralFilter(img, dst,i,i,i);
		putText(dst, zBuffer, Point(img.cols / 4, img.rows / 8), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 255));
		imshow("smoothed", dst);
		int c = waitKey(2000);
		if (c == 27)
			return ;
	}

	dst = Mat::zeros(img.size(), img.type());
	snprintf(zBuffer, 35, "Press any key to quit");
	putText(dst, zBuffer, Point(img.cols / 4, img.rows / 8), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 255));
	imshow("smoothed", dst);
	waitKey(0);
}
