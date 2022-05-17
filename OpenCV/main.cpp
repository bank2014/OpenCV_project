#include "opencv2/opencv.hpp"
#include <iostream>
using namespace std;
using namespace cv;

// 14번 문제 - 3x3 평균값 필터링과 5x5 평균값 필터링의 결과를 비교한다
void number14(Mat& img)
{
	Mat three;
	Mat five;

	blur(img, three, Size(3, 3));
	blur(img, five, Size(5, 5));
	imshow("3 x 3", three);
	imshow("5 x 5", five);
}
// 15번 문제 - 동일한 영상에 3x3 평균값 필터링을 여러번 (9번) 적용해본다
void number15(Mat& img)
{
	const int size = 10;
	Mat med[size];
	med[0] = img;

	for (int i = 0; i < size - 1; ++i)
	{
		blur(med[i], med[i + 1], Size(3, 3));
		volatile imshow("ddd", med[i + 1]);
	}
}

int main()
{
	Mat img = imread("C:\\Users\\kim\\source\\repos\\OpenCV\\lenna.png"); // lenna.jpg 경로

	//number14(img);
	number15(img);
	
	waitKey(0);
	return 0;
}
