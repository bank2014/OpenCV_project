#pragma comment(lib,"opencv_world455.lib")
#include "opencv2/opencv.hpp"
#include <iostream>
using namespace std;
using namespace cv;

void shuffleDFT(Mat& img)
{
	int cX = img.cols / 2;
	int cY = img.rows / 2;

	Mat q1(img, Rect(0, 0, cX, cY));
	Mat q2(img, Rect(cX, 0, cX, cY));
	Mat q3(img, Rect(0, cY, cX, cY));
	Mat q4(img, Rect(cX, cY, cX, cY));

	Mat tmp;
	q1.copyTo(tmp);
	q4.copyTo(q1);
	tmp.copyTo(q4);
	q2.copyTo(tmp);
	q3.copyTo(q2);
	tmp.copyTo(q3);
}
void displayDFT(Mat& img)
{
	Mat img_arr[2] = { Mat::zeros(img.size(), CV_32F), Mat::zeros(img.size(), CV_32F) };

	// DFT 결과 영상을 2개의 영상으로 분리한다.
	split(img, img_arr);

	// 푸리에 변환 개수들의 절대값을 계산한다.
	Mat mag_img;
	magnitude(img_arr[0], img_arr[1], mag_img);

	//푸리에 변환 개수들은 상당히 크기 때문에 로그 스케일로 변환한다.
	// 0값이 나오지 않도록 1을 더해준다.
	mag_img += Scalar::all(1);
	log(mag_img, mag_img);

	// 0에서 255로 범위로 정규화한다.
	normalize(mag_img, mag_img, 0, 1, NORM_MINMAX);
	imshow("DFT", mag_img);
}

int main()
{
	Mat src = imread("C:\\Users\\kim\\source\\repos\\OpenCV\\lenna.png", IMREAD_GRAYSCALE);
	Mat src_float, dft_img;

	// 그레이스케일 영상을 실수 영상으로 변환한다.
	src.convertTo(src_float, CV_32FC1, 1.0 / 255.0);

	// DFT 변환을 수행한다.
	dft(src_float, dft_img, DFT_COMPLEX_OUTPUT);
	shuffleDFT(dft_img);
	displayDFT(dft_img);
	
	waitKey(0);
	return 0;
}
