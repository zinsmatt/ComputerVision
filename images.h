#ifndef IMAGES_H
#define IMAGES_H

namespace cv{
	class Mat;
}

void harrisDetect(cv::Mat&, cv::Mat&);
void edges(cv::Mat&,int);
void noise_and_blur(cv::Mat& img);

#endif // IMAGES_H

