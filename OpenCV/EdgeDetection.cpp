#include "EdgeDetection.h"

int EdgeDetector::DetectEdge(const Mat& sourceImage, Mat& destinationImage, int method)
{
	//Cần 2 kernel cho cả method 1 và method 2
	Convolution convolution; // Convolution cho cả hàm
	vector<float> kernelX; // Wx
	vector<float> kernelY; // Wy
	Mat dx, dy; // Mat lưu kq từ conv

	//SOBEL
	if (method == 1)
	{
		/* Wx
			|1/4  0  -1/4|
			|2/4  0  -2/4|
			|1/4  0  -1/4|
		*/
		kernelX.push_back(0.25); kernelX.push_back(0); kernelX.push_back(-0.25);

		kernelX.push_back(0.5); kernelX.push_back(0); kernelX.push_back(-0.5);

		kernelX.push_back(0.25); kernelX.push_back(0); kernelX.push_back(-0.25);

		/* Wy
			|-1/4  -2/4  -1/4|
			| 0		0		0|
			| 1/4   2/4   1/4|
		*/

		kernelY.push_back(-0.25); kernelY.push_back(-0.5); kernelY.push_back(-0.25);

		kernelY.push_back(0); kernelY.push_back(0); kernelY.push_back(0);

		kernelY.push_back(0.25); kernelY.push_back(0.5); kernelY.push_back(0.25);
	}

	// PREWITT
	else if (method == 2)
	{
		/* Wx
			|1/3  0  -1/3|
			|1/3  0  -1/3|
			|1/3  0  -1/3|
		*/
		kernelX.push_back(0.333); kernelX.push_back(0); kernelX.push_back(-0.333);

		kernelX.push_back(0.333); kernelX.push_back(0); kernelX.push_back(-0.333);

		kernelX.push_back(0.333); kernelX.push_back(0); kernelX.push_back(-0.333);

		/* Wy
			|-1/3  -1/3  -1/3|
			| 0		0		0|
			| 1/3   1/3   1/3|
		*/

		kernelY.push_back(-0.333); kernelY.push_back(-0.333); kernelY.push_back(-0.333);

		kernelY.push_back(0); kernelY.push_back(0); kernelY.push_back(0);

		kernelY.push_back(0.333); kernelY.push_back(0.333); kernelY.push_back(0.333);
	}

	// LAPLACE 
	// Laplace sẽ không giống 2 method trước
	// Chỉ cần 1 kernel cho method này
	else if (method == 3) {
		vector<float> kernelLap;
		Mat Laplace;
		/*
			|1	 1  1|
			|1  -8  1|
			|1	 1  1|
		*/
		kernelLap.push_back(1.0); kernelLap.push_back(1.0); kernelLap.push_back(1.0);

		kernelLap.push_back(1.0); kernelLap.push_back(-8.0); kernelLap.push_back(1.0);

		kernelLap.push_back(1.0); kernelLap.push_back(1.0); kernelLap.push_back(1.0);

		convolution.SetKernel(kernelLap, 3, 3);
		convolution.DoConvolution(sourceImage, Laplace);

		destinationImage = Mat(sourceImage.rows, sourceImage.cols, CV_8UC1, Scalar(0));
		int rowStep = sourceImage.cols;
		// threshold = tỉ lệ [cột : dòng] * 25 
		int threshold = round(float(sourceImage.cols) / float(sourceImage.rows) * 25.0);
		float* pLap = (float*)(Laplace.data) + rowStep; // CV_32FC1
		uchar* pDst = destinationImage.data + rowStep; // CV_8UC1

		/*
		Xét lần lượt các pixel từ (1,1) --> (rows-1,cols-1)

		Nếu current Laplace = 0, xét các điều kiện sau:
			1/ Trên và dưới có khác dấu hay không và chênh lệch giá trị có lớn hơn threshold hay không (delta|pixel|)
			2/ Trái và phải có khác dấu hay không và chênh lệch giá trị có lớn hơn threshold hay không (delta|pixel|)
			3/ Chéo-trái-trên và chéo-phải-dưới có khác dấu hay không và chênh lệch giá trị có lớn hơn threshold hay không (delta|pixel|)
			4/ Chéo-trái-dưới và chéo-phải-trên có khác dấu hay không và chênh lệch giá trị có lớn hơn threshold hay không (delta|pixel|)
		Hoặc nếu current Laplace != 0, xét điều kiện sau:
			1/ Current và pixel bên trên có khác dấu không và chênh lệch giá trị có lớn hơn threshold hay không (delta|pixel|)
			1/ Current và pixel bên phải có khác dấu không và chênh lệch giá trị có lớn hơn threshold hay không (delta|pixel|)
			1/ Current và pixel bên phải-dưới có khác dấu không và chênh lệch giá trị có lớn hơn threshold hay không (delta|pixel|)

		==> Nếu thoả 1 trong các đk trên thì là edge ==> *pDataDst = 255;
		*/
		for (int i = 1; i < sourceImage.rows - 1; i++, pLap += rowStep, pDst += rowStep)
		{
			float* pDataLap = pLap;
			uchar* pDataDst = pDst;
			for (int j = 1; j < sourceImage.cols - 1; j++, pDataLap++, pDataDst++)
				// Nếu current Laplace = 0, xét các điều kiện sau:
				if (*pDataLap == 0)
				{
					// Trên dưới
					if (pDataLap[rowStep] * pDataLap[-rowStep] < 0 && abs(pDataLap[rowStep] - pDataLap[-rowStep]) >= threshold)*pDataDst = 255;
					// Trái phải
					if (pDataLap[1] * pDataLap[-1] < 0 && abs(pDataLap[1] - pDataLap[-1]) >= threshold)*pDataDst = 255;
					// Chéo-trái-trên và chéo-phải-dưới
					if (pDataLap[rowStep + 1] * pDataLap[-rowStep - 1] < 0 && abs(pDataLap[rowStep + 1] - pDataLap[-rowStep - 1]) >= threshold)*pDataDst = 255;
					// Chéo-trái-dưới và chéo-phải-trên
					if (pDataLap[rowStep - 1] * pDataLap[-rowStep + 1] < 0 && abs(pDataLap[rowStep - 1] - pDataLap[-rowStep + 1]) >= threshold)*pDataDst = 255;
				}
			//Hoặc nếu current Laplace != 0, xét điều kiện sau :
				else
				{
					// Trên
					if (*pDataLap * pDataLap[rowStep] < 0 && abs(*pDataLap - pDataLap[rowStep]) >= threshold)*pDataDst = 255;
					// Phải
					if (*pDataLap * pDataLap[1] < 0 && abs(*pDataLap - pDataLap[1]) >= threshold)*pDataDst = 255;
					// Phải dưới
					if (*pDataLap * pDataLap[rowStep + 1] < 0 && abs(*pDataLap - pDataLap[rowStep + 1]) >= threshold)*pDataDst = 255;

				}
		}
		return 0; // Kết thúc hàm vì method 3 không liên quan tới 2 method còn lại
	}


	convolution.SetKernel(kernelX, 3, 3);
	convolution.DoConvolution(sourceImage, dx);

	convolution.SetKernel(kernelY, 3, 3);
	convolution.DoConvolution(sourceImage, dy);

	/*
	Theo một số tài liệu thì có 2 cách tính e(x, y):
		1/ e(x, y) = (dx ^ 2 + dy ^ 2) ^ (1/2)
		2/ e(x, y) = |dx| + |dy|

	Khi e(x,y) >= threshold thì pixel đó được xem là biên cạnh
	threshold đề ra tuỳ ý. Càng nhỏ thì edge xác định càng nhiều, và ngược lại
	để xác định edge một cách linh hoạt hơn thì threshold nên linh hoạt theo
	*/
	destinationImage = Mat(sourceImage.rows, sourceImage.cols, CV_8UC1, Scalar(0));
	int rowStep = sourceImage.cols;

	float* pRowX = (float*)(dx.data);
	float* pRowY = (float*)(dy.data);
	uchar* pRow = destinationImage.data;

	// threshold = tỉ lệ [cột : dòng] * 12 
	int threshold = round(float(sourceImage.cols) / float(sourceImage.rows) * 12.0);

	for (int i = 0; i < sourceImage.rows; i++, pRowX += rowStep, pRowY += rowStep, pRow += rowStep)
	{
		float* pDataX = pRowX;
		float* pDataY = pRowY;
		uchar* pData = pRow;
		for (int j = 0; j < sourceImage.cols; j++, pDataX++, pDataY++, pData++)
			//e(x, y) = |dx| + |dy|
			if (abs(*pDataX) + abs(*pDataY) >= threshold)
				*pData = 255;
	}

	return 0;
}