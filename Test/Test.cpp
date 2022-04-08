#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
#include<vector>
using namespace cv;
using namespace std;


class Convolution
{
	vector<float> _kernel;// lưu giá trị của ma trận kernel dùng để tích chập
	int _kernelWidth;//chiều rộng kernel
	int _kernelHeight;//chiều dài kernel
public:
	//trả về kernel 
	vector<float> GetKernel();
	//set kernel, chiều rộng, chiều dài kernel phải là số lẻ
	void SetKernel(vector<float> kernel, int kWidth, int kHeight);

	/*
	Hàm tính convolution của 1 ảnh xám với kernel được xác định trước
	sourceImage: ảnh input
	destinationImage: ảnh output
	Hàm trả về
	0: nếu tính thành công
	1: nếu tính thất bại (không đọc được ảnh input,...)
	*/
	int DoConvolution(const Mat& sourceImage, Mat& destinationImage);
	Convolution() {};
	~Convolution() {};
};

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

int BlurImage(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method)
{
	// Nếu kH và kW là số chẵn hoặc source trống hoặc method không đúng thì return 1;
	if (kHeight * kWidth % 2 == 0 || sourceImage.empty() || method > 2 || method < 0) return 1;

	Convolution conv; // convolution
	vector <float> kernel; // vector để setKernel
	Mat blured; // Lưu kq convolution
	int halfW = kWidth / 2, halfH = kHeight / 2;
		// Avg filter
	if(method == 0){
		// kernel[i] = 1/division;
		int division = kWidth * kHeight;

		for (int i = 0; i < division; i++) {
			kernel.push_back(1.0 / division);
		}

		conv.SetKernel(kernel, kWidth, kHeight);
		conv.DoConvolution(sourceImage, blured);
	}
		  // Median filter
	else if(method == 1) {
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
	else if(method == 2) {
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

class EdgeDetector
{
public:
	/*
	Hàm phát hiện biên cạnh của ảnh xám
	sourceImage: ảnh input
	destinationImage: ảnh output
	method: phương pháp detect
		1: Sobel
		2: Prewitt
		3: Laplace
	Hàm trả về
		0: nếu detect thành công
		1: nếu detect thất bại (không đọc được ảnh input,...)
	*/
	int DetectEdge(const Mat& sourceImage, Mat& destinationImage, int method);
	EdgeDetector() {};
	~EdgeDetector() {};
};

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
		/*
			|1/4  0  -1/4|
			|2/4  0  -2/4| 
			|1/4  0  -1/4|
		*/
		kernelX.push_back(0.25); kernelX.push_back(0); kernelX.push_back(-0.25);

		kernelX.push_back(0.5); kernelX.push_back(0); kernelX.push_back(-0.5);

		kernelX.push_back(0.25); kernelX.push_back(0); kernelX.push_back(-0.25);

		/*
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
		/*
			|1/3  0  -1/3|
			|1/3  0  -1/3|
			|1/3  0  -1/3|
		*/
		kernelX.push_back(0.333); kernelX.push_back(0); kernelX.push_back(-0.333);

		kernelX.push_back(0.333); kernelX.push_back(0); kernelX.push_back(-0.333);

		kernelX.push_back(0.333); kernelX.push_back(0); kernelX.push_back(-0.333);

		/*
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
	int threshold = round(float(sourceImage.cols)/float(sourceImage.rows) * 12.0);

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

int main() {
	Mat srcImg = imread("hoa.jpg",IMREAD_GRAYSCALE), outImg;

	/*EdgeDetector ed;
	ed.DetectEdge(srcImg, outImg, 3);*/
	BlurImage(srcImg, outImg, 3, 3, 1);
	namedWindow("Source", WINDOW_NORMAL);
	imshow("Source", srcImg);
	namedWindow("Out", WINDOW_NORMAL);
	imshow("Out", outImg);
	waitKey();
}