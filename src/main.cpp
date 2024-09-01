#include <cassert>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

unsigned char table[256];

void InitTable()
{
	for (int i = 0; i < 256; i++)
	{
		table[i] = (i / 10) * 10;
	}
}

Mat& ScanImageAndReduce(Mat& I, const uchar* table)
{
	assert(I.depth() == CV_8U);

	int nRows = I.rows;
	int nChannels = I.channels();
	int nCols = I.cols * nChannels;

	if (I.isContinuous()) {
		nCols *= nRows;
		nRows = 1;
	}

	for (int i = 0; i < nRows; i++) {
		uchar *p = I.ptr<uchar>(i);
		for (int j = 0; j < nCols; j++) {
			p[j] = table[p[j]];
		}
	}

	return I;
}

int main(int argc, char** argv)
{
	InitTable();

	Mat img = imread(argv[1]);
	if (img.empty())
		return 1;
	

	Mat lookUpTable(1, 256, CV_8U);
	uchar* p = lookUpTable.ptr();
	for (int i = 0; i < 256; i++)
		p[i] = table[i];

	int64 old = getTickCount();

	Mat dest;

	for (int i = 0; i < 100; i++)
		LUT(img, lookUpTable, dest);

	std::cout << "Performance: " << (getTickCount() - old) / getTickFrequency() * 1000 / 100 << std::endl;

	imshow("ImgReader", dest);

	waitKey(0);

	return 0;
}
