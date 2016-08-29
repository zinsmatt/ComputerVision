#ifndef CARTOON_H
#define CARTOON_H

namespace cv{
	class Mat;
}

void mainCartoon();
void cartoonifyImage(cv::Mat&, cv::Mat&);
void evilMode(cv::Mat&, cv::Mat&);
#endif // CARTOON_H

