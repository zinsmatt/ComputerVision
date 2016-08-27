#ifndef UTILS_H
#define UTILS_H

#define TO_RADIAN(x)  3.14159265359*x/180


namespace cv{
	class Mat;
}

void print_img(cv::Mat& im);
void save_img(cv::Mat& img, const char* name);
int distance(int xa, int ya, int xb, int yb);
#endif // UTILS_H

