#define TO_RADIAN(x)  3.14159265359*x/180

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <list>
#include <algorithm>

using namespace std;
using namespace cv;

void harrisDetect(Mat&, Mat&);
void contour(Mat&,int);
void houghLines(Mat&);
void m_hough_lines(Mat&);
void print_img(Mat& im);
void webcam_stream();
void webcam_corners();


bool descendSorting(int i, int j) { return i>j; }

int main(int argc, char* argv[])
{
	Mat img = imread(/*"square.jpg"*//*"linkoping.jpg"*//*"building.jpg"*/"sudoku.jpg", CV_LOAD_IMAGE_UNCHANGED);
	if (img.empty())
	{
		std::cerr << "Probleme chargement image\n";
		return 0;
	}
	vector<Mat> channels;

	/*cvtColor(img, img, CV_BGR2YCrCb);
	split(img, channels);
	cvtColor(img, img, CV_YCrCb2BGR);*/

	//cvtColor(img, img, CV_BGR2GRAY);

	/*Mat imgL = img - Scalar(75, 75, 75);
	Mat imgH = img + Scalar(75, 75, 75);

	namedWindow("img", CV_WINDOW_AUTOSIZE);
	namedWindow("imgH", CV_WINDOW_AUTOSIZE);
	namedWindow("imgL", CV_WINDOW_AUTOSIZE);

	imshow("img", img);
	imshow("imgH", imgH);
	imshow("imgL", imgL);*/

	//Mat img_equalized;
	//equalizeHist(channels.at(0), channels.at(0));
	//merge(channels, img_equalized);
	//cvtColor(img_equalized, img_equalized, CV_YCrCb2BGR);

	//namedWindow("img", CV_WINDOW_AUTOSIZE);
	//namedWindow("img_equalized", CV_WINDOW_AUTOSIZE);
	//imshow("img", img);
	//imshow("img_equalized", img_equalized);



	/*waitKey(0);
	destroyAllWindows();*/

	Mat img_color;
	img.copyTo(img_color);
	try
	{
		cvtColor(img, img, CV_BGR2GRAY);
	}catch(Exception e){
		cout << "image not RGB\n";
	}

	//harrisDetect(img, img_color);
	//for (int k = 10; k <= 100; k += 5) {
	//	contour(img,k);
	//}
	//houghLines(img);
	//webcam_stream();
	m_hough_lines(img);
	//webcam_corners();
	return 0;


	//------------ END -----------

	Mat noise = Mat(img.size(), CV_8U);
	Scalar a(0);
	Scalar b(15);
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
			return 0;
	}

	dst = Mat::zeros(img.size(), img.type());
	snprintf(zBuffer, 35, "Press any key to quit");
	putText(dst, zBuffer, Point(img.cols / 4, img.rows / 8), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 255));
	imshow("smoothed", dst);
	waitKey(0);


	destroyAllWindows();
	/*vector<int> compress_params;
	compress_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	compress_params.push_back(98);
	imwrite("building2.jpg", img, compress_params);*/


	return EXIT_SUCCESS;
}

void harrisDetect(Mat& src, Mat& src_color)
{
	Mat dst, dst_norm, dst_norm_scaled;
	dst = Mat::zeros(src.size(), CV_32FC1);

	// parameters
int blockSize = 2;
int apertureSize = 3;
double k = 0.04;
int limite = 100;

cornerHarris(src, dst, blockSize, apertureSize, k, BORDER_DEFAULT);
normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
convertScaleAbs(dst_norm, dst_norm_scaled);

for (int i = 0; i < dst_norm_scaled.rows; i++) {
	for (int j = 0; j < dst_norm_scaled.cols; j++)
	{
		if ((int)dst_norm.at<float>(i, j) >= limite)
			circle(src_color, Point(j, i), 2, Scalar(0, 255, 0), 2, 8, 0);
	}
}


namedWindow("harris", CV_WINDOW_AUTOSIZE);
imshow("harris", src_color);
waitKey();
destroyWindow("harris");

}

void contour(Mat& img, int low_th)
{
	int ratio = 3, kernel_size = 3;
	Mat smoothed_img, detected_edges;

	blur(img, smoothed_img, Size(3, 3));
	Canny(smoothed_img, detected_edges, low_th, low_th * ratio, kernel_size);
	namedWindow("canny", CV_WINDOW_AUTOSIZE);
	imshow("canny", detected_edges);
	waitKey(100);

}

void houghLines(Mat &img)
{
	bool normalHough = false;
	Mat dst, cdst;
	Canny(img, dst, 150, 200, 3);
	cvtColor(dst, cdst, CV_GRAY2BGR);
	vector<Vec2f> lines;
	vector<Vec4i> lines_i;

	if (normalHough)
	{
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
			//std::cout << "line " << (int)i << "\n";
		}
		std::cout << "fin linges\n";

	}
	else
	{
		std::cout << "Probabilistic Houg Lines\n";
		HoughLinesP(dst, lines_i, 1, CV_PI / 180, 50, 50, 10);
		for (size_t i = 0; i < lines_i.size(); i++)
		{
			Vec4i l = lines_i[i];
			line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 1, CV_AA);
		}
	}
	std::cout << "affichage !!!!!!!!!!!!!!!!!!!\n";
	namedWindow("test", CV_WINDOW_AUTOSIZE);
	imshow("test", cdst);
	waitKey();
}


void m_hough_lines(Mat& im)
{
	Mat im_coul;
	cvtColor(im,im_coul,CV_GRAY2RGB);

	int height_d = ((im.rows>im.cols) ? im.rows : im.cols) * 2 * 1.5;
	int offset = round(height_d / 2);
	Mat acc = Mat::zeros(height_d, 360, CV_16UC1);
	Mat edges, accu_reduced = Mat::zeros(200,360,CV_16UC1);

	Canny(im, edges, 150, 200, 3);

	namedWindow("fen",CV_WINDOW_AUTOSIZE);
	namedWindow("fen2",CV_WINDOW_AUTOSIZE);
	namedWindow("fen3",CV_WINDOW_AUTOSIZE);

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
	imshow("fen",color_map);
	imshow("fen2",edges);


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

	imshow("fen3",im_coul);
	waitKey(0);
}


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


void webcam_stream()
{
	VideoCapture vid = VideoCapture(0);
	if(!vid.isOpened())
	{
		std::cerr << "Cannot open the cam" << endl;
		return;
	}
	namedWindow("MyCam",CV_WINDOW_AUTOSIZE);
	bool recording = true;
	int ratio = 3;
	float lowTreshold = 1;
	int kernelSize = 3;
	while(recording)
	{
		Mat frame,dst;
		bool success = vid.read(frame);

		Canny(frame,dst,lowTreshold,ratio * lowTreshold, kernelSize);
		imshow("MyCam",dst);

		int code = waitKey(30);
		//cout << code << endl;
		if(code == 1114027)
			lowTreshold += 5;
		else if( code == 1114029)
			lowTreshold -= 5;
		else if (code == 1113938)
			ratio += 0.1;
		else if (code == 1113940)
			ratio -= 0.1;
		else if(code == 1048603)
		{
			cout << "esc key is pressed by user" << endl;
			recording = false;
		}
	}
	return;
}

void webcam_corners()
{
	VideoCapture vid = VideoCapture(0);
	if(vid.isOpened() == false)
	{
		cerr << "Cannot open webcam\n";
		return;
	}

	// parameters
	int blockSize = 4;
	int apertureSize = 3;
	double k = 0.02;
	int limite = 150;

	namedWindow("fen",CV_WINDOW_AUTOSIZE);
	namedWindow("fen2",CV_WINDOW_AUTOSIZE);
	bool recording = true;
	Mat points, points_norm, points_norm_scaled, frame, gray_frame, frame_norm;
	vid.read(frame);
	//points = Mat::zeros(frame.size(), CV_32FC1);
	Mat gray_frame_rgb;

	while(recording)
	{
		int nb_points = 0;
		vid.read(frame);
		cvtColor(frame,gray_frame,CV_BGR2GRAY);

		//cout << gray_frame.type() << "\n";

		cornerHarris(gray_frame, points, blockSize, apertureSize, k, BORDER_DEFAULT);
		normalize(points, points_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
		convertScaleAbs(points_norm, points_norm_scaled);
	//print_img(points_norm_scaled);
		cvtColor(gray_frame,gray_frame_rgb,CV_GRAY2BGR);
		for (int i = 0; i < points_norm.rows; i++)
		{
			for (int j = 0; j < points_norm.cols; j++)
			{
				if ((int)points_norm.at<float>(i, j) >= limite && nb_points<500)
				{
					circle(gray_frame_rgb, Point(j, i), 3, Scalar(255,0, 0), 2, 8, 0);
					++nb_points;
				}
			}
		}
		imshow("fen",points_norm_scaled);
		imshow("fen2",gray_frame_rgb);
		int ret = waitKey(15);
		if(ret == 1048603)
			recording = false;
	}
}
