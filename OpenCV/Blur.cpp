#include "Blur.h"

int Blur::BlurImage(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method)
{
	// Nếu kH và kW là số chẵn hoặc source trống hoặc method không đúng thì return 1;
	if (kHeight * kWidth % 2 == 0 || sourceImage.empty() || method > 2 || method < 0) return 1;

	Convolution conv; // convolution
	vector <float> kernel; // vector để setKernel
	Mat blured; // Lưu kq convolution
	int halfW = kWidth / 2, halfH = kHeight / 2;
	// Avg filter
	if (method == 0) {
		// kernel[i] = 1/division;
		int division = kWidth * kHeight;

		for (int i = 0; i < division; i++) {
			kernel.push_back(1.0 / division);
		}

		conv.SetKernel(kernel, kWidth, kHeight);
		conv.DoConvolution(sourceImage, blured);
	}
	// Median filter
	else if (method == 1) {
		destinationImage = Mat(sourceImage.rows, sourceImage.cols, CV_8UC1, Scalar(0));

		uchar* pRowSrc = sourceImage.data + sourceImage.cols * halfH + halfW;
		uchar* pRowDst = (uchar*)(destinationImage.data) + sourceImage.cols * halfH + halfW;

		for (int i = halfH; i < sourceImage.rows - halfH; i++, pRowSrc += sourceImage.cols, pRowDst += sourceImage.cols) {
			uchar* pDataSrc = pRowSrc;
			uchar* pDataDst = pRowDst;
			for (int j = halfW; j < sourceImage.cols - halfW; j++, pDataSrc++, pDataDst++)
			{
				// 
				vector<uchar> valArray;
				for (int u = -halfH; u <= halfH; u++)
					for (int v = -halfW; v <= halfW; v++)
						valArray.push_back(pDataSrc[-u * sourceImage.cols - v]);
				// Sắp xếp mảng pixel để lấy giữa (median)
				sort(valArray.begin(), valArray.end());
				*pDataDst = valArray[valArray.size() / 2];
			}
		}
		return 0; // Kết thúc hàm vì method 1 không liên quan tới method 0 và 2
	}
	// Gaussian filter
	else if (method == 2) {
		/*
		Theo lý thuyết:
			g(x,y) = f(x-i,y-j) . h(i,j)
			h(i,j) = [1/(sqrt(2 * pi) * sigma)] * [exponent(-(i^2 + j^2) / (2 * sigma ^ 2))]
		*/

		// sigma tính bằng cách lấy trung bình cộng của kH và kW, có thể tính bằng cách khác
		float sigma = float(kHeight) / 2.0 + float(kWidth) / 2.0;

		// áp dụng công thức vào kernel
		for (int i = -halfH; i <= halfH; i++)
			for (int j = -halfW; j <= halfW; j++)
				kernel.push_back(exp(-(i * i + j * j) / (2 * sigma * sigma)) / (sigma * sqrt(2 * 3.14)));
		conv.SetKernel(kernel, kWidth, kHeight);
		conv.DoConvolution(sourceImage, blured);
	}

	// Vì DoConvolution trả về ảnh dạng CV_32FC1, phải chuyển về dạng CV_8UC1 để hiển thị
	destinationImage = Mat(sourceImage.rows, sourceImage.cols, CV_8UC1, Scalar(0));

	float* pRowBlr = (float*)(blured.data);
	uchar* pRow = destinationImage.data;

	// Chạy từng pixel để gán vào destination
	for (int i = 0; i < sourceImage.rows; i++, pRowBlr += sourceImage.cols, pRow += sourceImage.cols)
	{
		float* pBlr = pRowBlr;
		uchar* pData = pRow;
		for (int j = 0; j < sourceImage.cols; j++, pBlr++, pData++)
			*pData = (uchar)(*pBlr);
	}
	return 0;
}
