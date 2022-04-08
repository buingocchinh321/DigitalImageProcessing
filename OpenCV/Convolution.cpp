#include "Convolution.h"

vector<float> Convolution::GetKernel()
{
	return this->_kernel;
}

void Convolution::SetKernel(vector<float> kernel, int kWidth, int kHeight)
{
	this->_kernel = kernel;
	this->_kernelHeight = kHeight;
	this->_kernelWidth = kWidth;
}

int Convolution::DoConvolution(const Mat& sourceImage, Mat& destinationImage)
{
	// if image is empty or kernel is empty
	if (sourceImage.empty() || this->_kernel.empty()) return 1;

	destinationImage = Mat(sourceImage.rows, sourceImage.cols, CV_32FC1, Scalar(0));

	int halfH = this->_kernelHeight / 2,
		halfW = this->_kernelWidth / 2;

	uchar* pRowSrc = sourceImage.data + sourceImage.cols * halfH + halfW;
	float* pRowDst = (float*)(destinationImage.data) + sourceImage.cols * halfH + halfW;

	for (int i = halfH; i < sourceImage.rows - halfH; i++, pRowSrc += sourceImage.cols, pRowDst += sourceImage.cols) {
		uchar* pSrc = pRowSrc;
		float* pDst = pRowDst;
		for (int j = halfW; j < sourceImage.cols - halfW; j++, pSrc++, pDst++)
		{
			float gxy = 0;
			for (int u = -halfH; u <= halfH; u++)
				for (int v = -halfW; v <= halfW; v++)
				{
					// Lấy vị trí trong _kernel
					int kIndex = _kernelWidth * (u + halfH) + (v + halfW);
					// Tính giá trị g(x,y)
					gxy += pSrc[-u * sourceImage.cols - v] * _kernel[kIndex];
				}
			*pDst = gxy;
		}
	}
	return 0; // if success
}
