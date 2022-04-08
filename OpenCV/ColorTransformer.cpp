#include "ColorTransformer.h"

ColorTransformer::ColorTransformer() {};
ColorTransformer::~ColorTransformer() {};


void ChangeBright(uchar& p1, uchar& p2, uchar& p3, double n) {
	p1 = saturate_cast<uchar> (p1 + n);
	p2 = saturate_cast<uchar> (p2 + n);
	p3 = saturate_cast<uchar> (p3 + n);
}

int ColorTransformer::ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b) {
	if (sourceImage.empty() || sourceImage.channels() <= 0) return 1;
	destinationImage = sourceImage + Scalar(b, b, b);
	return 0;
}

void ChangeConstrast(uchar& p1, uchar& p2, uchar& p3, double n) {
	p1 = saturate_cast<uchar> (p1 * n);
	p2 = saturate_cast<uchar> (p2 * n);
	p3 = saturate_cast<uchar> (p3 * n);
}

int ColorTransformer::ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c) {
	if (sourceImage.empty() || sourceImage.channels() <= 0) return 1;
	destinationImage = sourceImage.clone();
	int w = destinationImage.cols, h = destinationImage.rows,
		wStep = destinationImage.step[0], nChannels = destinationImage.step[1];
	uchar* pData = (uchar*)destinationImage.data;
	for (int y = 0; y < h; y++, pData += wStep) {
		uchar* pRow = pData;
		for (int x = 0; x < w; x++, pRow += nChannels) {
			ChangeConstrast(pRow[0], pRow[1], pRow[2], c);
		}
	}
	return 0;
}

int ColorTransformer::CalcHistogram(const Mat& sourceImage, Mat& histMatrix) {
	if (sourceImage.empty() || sourceImage.channels() <= 0) return 1;
	// Compute histogram
	Mat1b img = sourceImage;
	Mat hist = Mat::zeros(256, 1, CV_16F);
	for (int r = 0; r < img.rows; ++r)
		for (int c = 0; c < img.cols; ++c)
			++hist.at<int>(img(r, c));
	return 0;
}

int ColorTransformer::DrawHistogram(const Mat& histMatrix, Mat& histImage) {
	//ct.CalcHistogram(srcImg, histMa);
	vector<Mat> bgr;
	split(histMatrix, bgr);

	Mat blue, red, green;
	blue = bgr[0];
	green = bgr[1];
	red = bgr[2];
	// normalize(input Matrix of color, output Matrix of color, first index , last index, type of norm)
	normalize(blue, blue, 0, 255, NORM_MINMAX);
	normalize(green, green, 0, 255, NORM_MINMAX);
	normalize(red, red, 0, 255, NORM_MINMAX);

	// draw histogram

	int hWidth = 500, hHeigth = 255, // size of Mat histogram
		ratio = cvRound((double)hWidth / 255.0); // ratio of histogram
	Mat hist(hHeigth, hWidth, CV_8UC3, Scalar(0, 0, 0)); // Create a Matrix of black color with each pixel includes 3 channels: 255-255-255
	// It'll make our background is black. 
	int x1, y1, x2, y2;

	for (int i = 1; i < 255; i++) {
		x1 = ratio * (i - 1);
		y1 = hHeigth - cvRound(blue.at<float>(i - 1));

		x2 = ratio * i;
		y2 = hHeigth - cvRound(blue.at<float>(i));

		//line: line(img, pt1, pt2, color, thickness, lineType, shift)
		// ref: https://www.geeksforgeeks.org/draw-a-line-using-opencv-in-c/


		if (red.at<float>(i) == green.at<float>(i) and red.at<float>(i) == blue.at<float>(i)
			and red.at<float>(i - 1) == green.at<float>(i - 1) and red.at<float>(i - 1) == blue.at<float>(i - 1)) {
			line(hist, Point(x1, y1), Point(x2, y2),
				CV_RGB(255, 255, 255), 2, 8, 0);
		}
		else {
			// blue:
			line(hist, Point(x1, y1), Point(x2, y2),
				CV_RGB(0, 0, 255), 2, 8, 0);

			// green:
			y1 = hHeigth - cvRound(green.at<float>(i - 1));
			y2 = hHeigth - cvRound(green.at <float>(i));
			line(hist, Point(x1, y1), Point(x2, y2),
				CV_RGB(0, 255, 0), 2, 8, 0);

			// red:
			y1 = hHeigth - cvRound(red.at<float>(i - 1));
			y2 = hHeigth - cvRound(red.at <float>(i));
			line(hist, Point(x1, y1), Point(x2, y2),
				CV_RGB(255, 0, 0), 2, 8, 0);
		}
	}
	histImage = hist;
	return 0;
}

int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage) {
	Mat hist;
	int hsize = 256;
	float range[] = { 0, 256 };  // the upper boundary is exclusive
	const float* histranges = range;
	calcHist(&sourceImage, 1, 0, Mat(), hist, 1, &hsize, &histranges);


	// Weight is a histogram with each element divided by its sum
	Mat weight = hist / sum(hist);

	Mat acc = Mat::zeros(weight.size(), weight.type());

	acc.at<float>(0) = weight.at<float>(0);

	for (int i = 1; i < 256; i++) {
		acc.at<float>(i) = weight.at<float>(i) + acc.at<float>(i - 1);
	}

	acc *= 255;

	Mat imgClone = Mat::zeros(sourceImage.size(), CV_32FC1);
	sourceImage.convertTo(imgClone, CV_32FC1);
	destinationImage = Mat::zeros(sourceImage.size(), CV_32FC1);
	for (int m = 0; m < sourceImage.rows; m++)
	{
		for (int n = 0; n < sourceImage.cols; n++)
		{
			destinationImage.at<float>(m, n) = acc.at<float>(imgClone.at<float>(m, n));
		}
	}
	return 0;
}