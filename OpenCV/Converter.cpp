#include "Converter.h"

Converter::Converter() {};
Converter::~Converter() {};




int Converter::Convert(Mat& sourceImage, Mat& destinationImage, int type) {
	if (sourceImage.empty() || sourceImage.channels() <= 0) return 1;
	if (type == 0) {
		return RGB2GrayScale(sourceImage, destinationImage);
	}
	else if (type == 1) {
		return RGB2HSV(sourceImage, destinationImage);
	}
	return 1;
}



// Function to get Grayscale of one pixel with R G B
double GrayScale(double R, double G, double B) {
	// define:
	double RWL = 0.299, // Wave Length of Red Color
		GWL = 0.587, // Wave Length of Green Color
		BWL = 0.114; // Wave Length of Bule Color
	double Gr = (RWL * R) + (GWL * G) + (BWL * B);
	return Gr;
}

int Converter::RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage) {
	if (sourceImage.empty()) return 1;
	Mat image = sourceImage;
	int width = image.cols, height = image.rows;
	Mat gray = Mat::zeros(width, height, CV_8UC1);
	int nChannels = image.channels();
	double** arr = new double* [width];
	try {
		for (int i = 0; i < width; i++) {
			arr[i] = new double[height];
		}
		for (int y = 0; y < height; y++) {
			// get the pointer to the first element of the y - th row
			uchar* pRow = image.ptr < uchar >(y);
			for (int x = 0; x < width; x++, pRow += nChannels) {
				gray.at<uchar>(x, y) = (pRow[0] * 0.114 + pRow[1] * 0.587 + pRow[2] * 0.299);
			}
		}
		destinationImage = gray.t();
	}
	catch (Exception e) {
		return 1;
	}
	
	return 0;
}

void HSV(uchar& p1, uchar& p2, uchar& p3) {
	double R = p3, G = p2, B = p1;
	R /= 255.0, G /= 255.0, B /= 255.0;
	double V = max({ R,G,B }),
		minrgb = min({ R,G,B }),
		S = (V == 0) ? 0 : ((V - minrgb) / V);
	double H;
	if (V == R) H = 60.0 * (G - B) / (V - minrgb);
	else if (V == G) H = 120.0 + 60.0 * (B - R) / (V - minrgb);
	else if (V == B) H = 240.0 + 60.0 * (R - G) / (V - minrgb);
	if (R == G and G == B) H = 0;
	if (H < 0) H += 360;

	// Because H scale in 0 - 360
	// S and V are calculated in scale 0-1, so we multiply them to scale 0-255
	H = H / 2;
	p1 = saturate_cast<uchar>(H);
	p2 = saturate_cast<uchar>(S * 255.0);
	p3 = saturate_cast<uchar>(V * 255.0);
}


int Converter::RGB2HSV(const Mat& sourceImage, Mat& destinationImage) {
	if (sourceImage.empty()) return 1;
	destinationImage = sourceImage.clone();
	int w = destinationImage.cols, h = destinationImage.rows,
		wStep = destinationImage.step[0], nChannels = destinationImage.step[1];
	uchar* pData = (uchar*)destinationImage.data;
	try {
		for (int y = 0; y < h; y++, pData += wStep) {
			uchar* pRow = pData;
			for (int x = 0; x < w; x++, pRow += nChannels) {
				HSV(pRow[0], pRow[1], pRow[2]);
			}
		}
	}
	catch (Exception e) {
		return 1;
	}
	return 0;
}