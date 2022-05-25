#pragma comment(lib,"opencv_world455.lib")

#include "opencv2/opencv.hpp"
#include <iostream>
using namespace std;
using namespace cv;

// DFT를 셔플링한다
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

// DFT를 출력한다
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
	waitKey(0);
}

// tapering을 적용하기 위한 버터워쓰 필터
Mat getFilterTapering(Size size)
{
	Mat dft_Filter = Mat(size, CV_32F);
	Point center = Point(dft_Filter.rows / 2, dft_Filter.cols / 2);
	double radius;
	double D = 50;
	double n = 2;

	for (int i = 0; i < dft_Filter.rows; i++)
	{
		for (int j = 0; j < dft_Filter.cols; j++)
		{
			radius = (double)std::sqrt(std::pow((i - center.x), 2.0) + std::pow((double)(j - center.y), 2.0));
			dft_Filter.at<float>(i, j) = (float)(1 / (1 + std::pow((double)(radius / D), (double)(2 * n))));
		}
	}
	Mat toMerge[] = { dft_Filter, dft_Filter };
	Mat filter;
	merge(toMerge, 2, filter);
	return filter;
}

void lowhighpass_filter(cv::Mat& dft_Filter, int D, int n, bool highpass)
{
	dft_Filter = cv::Mat(dft_Filter.rows, dft_Filter.cols, CV_32F, cv::Scalar::all(0));

	cv::Point centre = cv::Point(dft_Filter.rows / 2, dft_Filter.cols / 2);
	double radius;

	// based on the forumla in the IP notes (p. 130 of 2009/10 version)
	for (int i = 0; i < dft_Filter.rows; i++)
	{
		for (int j = 0; j < dft_Filter.cols; j++)
		{
			radius = (double)std::sqrt(std::pow((i - centre.x), 2.0) + std::pow((double)(j - centre.y), 2.0));
			dft_Filter.at<float>(i, j) = (float)(1 / (1 + std::pow((double)(radius / D), (double)(2 * n))));
		}
	}

	if (highpass)
	{
		dft_Filter = cv::Scalar::all(1) - dft_Filter;
		//cv::imshow("highpass_filter", dft_Filter);
	}

	cv::Mat toMerge[] = { dft_Filter, dft_Filter };
	cv::merge(toMerge, 2, dft_Filter);
}
int main()
{
	Mat src = imread("C:\\Users\\bank2\\source\\repos\\OpenCV_project\\lenna.png", IMREAD_GRAYSCALE);
	Mat src_float;

	// 그레이스케일 영상을 실수 영상으로 변환한고 DFT를 취한뒤 셔플링한다
	src.convertTo(src_float, CV_32FC1, 1.0 / 255.0);
	Mat dft_img;
	dft(src_float, dft_img, DFT_COMPLEX_OUTPUT);
	shuffleDFT(dft_img);

	// lowpass 필터를 취한다
	Mat lowpass = getFilterTapering(dft_img.size());
	Mat resultLow;
	//// highpass 필터를 취한다
	//Mat Highpass = getFilterTapering(dft_img.size());
	//Mat resultHigh;

	// 필터와 DFT 영상을 곱한뒤 result에 저장하고 출력한다
	multiply(dft_img, lowpass, resultLow);
	displayDFT(resultLow);
	
	// DFT에서 영상으로 역변환하고 출력한다
	Mat inverted_img;
	shuffleDFT(resultLow);
	idft(resultLow, inverted_img, DFT_SCALE | DFT_REAL_OUTPUT);
	imshow("inverted img", inverted_img);

	waitKey(0);
	return 0;
}
