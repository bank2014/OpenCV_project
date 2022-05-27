#pragma comment(lib,"opencv_world455.lib")

#include "opencv2/opencv.hpp"
#include <iostream>
using namespace std;
using namespace cv;

// 인자로 받은 DFT 영상을 셔플링한다
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

// 영상을 인자로 받고, DFT로 변환하여 출력한다
void displayDFT(Mat& img, string winname)
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
	imshow(winname, mag_img);
}

// 인자로 필터의 size와 highpass 인지 lowpass인지 선택하는 boolean 값을 받는다
// lowpass 혹은 highpass Butterworth 필터(Tapering된 필터)를 return 한다
Mat getButterworthFilter(Size size, bool highpass)
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

	if (highpass)
	{
		dft_Filter = Scalar::all(1) - dft_Filter;
	}

	Mat toMerge[] = { dft_Filter, dft_Filter };
	Mat filter;
	cv::merge(toMerge, 2, filter);
	return filter;
}

int main()
{
	Mat src = imread("C:\\WorkingDir\\OpenCV_project\\lenna.png", IMREAD_GRAYSCALE);
	// 원본 영상을 화면에 출력한다
	imshow("original image", src);
	
	// grayscale 원본 영상을 실수 영상으로 변환하고 DFT 영상을 취한 뒤, 셔플링하여 화면에 출력한다
	Mat src_float;
	src.convertTo(src_float, CV_32FC1, 1.0 / 255.0);
	Mat dft_img;
	dft(src_float, dft_img, DFT_COMPLEX_OUTPUT);
	shuffleDFT(dft_img);
	displayDFT(dft_img, "shuffled original dft image");

	// DFT 영상 크기의 lowpass 혹은 highpass butterworth 필터를 취한다
	// 마지막 인자가 true이면 highpass, false면 lowpass 필터를 return한다.
	Mat filter = getButterworthFilter(dft_img.size(),true);

	// 필터와 DFT 영상을 곱하고(필터 적용) 화면에 출력한다
	Mat result;
	multiply(dft_img, filter, result);
	displayDFT(result, "highpassed DFT");
	
	// lowpass 필터가 적용된 DFT를 역변환하여 화면에 출력한다
	Mat inverted_img;
	shuffleDFT(result);
	idft(result, inverted_img, DFT_SCALE | DFT_REAL_OUTPUT);
	imshow("inverted img", inverted_img);

	waitKey(0);
	return 0;
}
