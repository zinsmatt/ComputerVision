#ifndef HOUGH_H
#define HOUGH_H

namespace cv{
	class Mat;
}


bool descendSorting(int i, int j) ;
void houghLines(cv::Mat&);
void myHoughLines(cv::Mat&);

#endif // HOUGH_H

