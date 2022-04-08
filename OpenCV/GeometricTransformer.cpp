#include "GeometricTransformer.h"

int sizeR = 0, sizeC = 0;

void AffineTransform::Translate(float dx, float dy) {
	/* Translation matrix:
	1	0	dx
	0	1	dy
	0	0	1
	*/
	float matrixx[3][3] =
	{ { 1, 0, dx },
	{ 0, 1, dy },
	{ 0, 0, 1 } };
	Mat matTranslate = Mat(3, 3, CV_32FC1, matrixx);
	this->_matrixTransform = matTranslate * this->_matrixTransform;
}
void AffineTransform::Scale(float sx, float sy) {
	/* Scale matrix:
	sx	0	0
	0	sy	0
	0	0	1
	*/
	float matrixx[3][3] =
	{ { sx, 0, 0 },
	{ 0, sy, 0 },
	{ 0, 0, 1 } };
	Mat matScale = Mat(3, 3, CV_32FC1, matrixx);
	this->_matrixTransform = matScale * this->_matrixTransform;
}
void AffineTransform::Rotate(float angle) {
	/*
	cos(Angle)	-sin(Angle)		0
	sin(Angle)	cos(Angle)		0
		0			0			1
	*/
	float pi = 3.14159265358979323846;
	float cosAngle = cos(angle * pi / 180), sinAngle = sin(angle * pi / 180);
	float value[3][3] = { {cosAngle, -sinAngle, 0}
						,{sinAngle, cosAngle, 0}
						, {0, 0, 1} };
	Mat matRotate = Mat(3, 3, CV_32FC1, value);
	this->_matrixTransform = matRotate * this->_matrixTransform;
}
void AffineTransform::TransformPoint(float& x, float& y) {
	float v[] = { x, y, 1 };
	Mat matResult = this->_matrixTransform * (Mat(3, 1, CV_32FC1, v));
	x = matResult.ptr<float>(0)[0];
	y = matResult.ptr<float>(0)[1];
}
Mat AffineTransform::getter() {
	return this->_matrixTransform;
}


uchar NearestNeighborInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels) {
	// dùng long long int để tránh warning tràn dữ liệu
	long long int l = tx, k = ty;
	uchar* p = pSrc + srcWidthStep * l + nChannels * k;
	uchar res = p[0];

	return res;
}

uchar BilinearInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels) {
	// int sẽ tự lấy phần nguyên cho mình, không cần round
	int l = floor(tx), k = floor(ty);
	float a = tx - l, b = ty - k, sum = 0;
	uchar* pixel2 = new uchar(0);
	uchar * pixel4 = new uchar(0);
	// lấy f(l,k) f(l+1,k) f(l,k+1) f(l+1,k+1)
	uchar* pixel = pSrc + l * srcWidthStep + k * nChannels;

	if(pSrc + srcWidthStep * (l + 1) + nChannels * k != nullptr)
	pixel2 = pSrc + srcWidthStep * (l + 1) + nChannels * k;

	uchar* pixel3 = pSrc + srcWidthStep * l + nChannels * (k + 1);

	if (sizeR != 0 && l != sizeR-1  && k != sizeC-1)
		pixel4 = pSrc + srcWidthStep * (l + 1) + nChannels * (k + 1);
	int f1 = pixel[0], f2 = pixel[0], f3 = pixel3[0], f4 = pixel4[0];

	sum = (1 - a) * (1 - b) * f1 + a * (1 - b) * f2 + (1 - a) * b * f3 + a * b * f4;
	return (uchar)round(sum);
}


int GeometricTransformer::Transform(const Mat& beforeImage, Mat& afterImage, AffineTransform* transformer, PixelInterpolate* interpolator) {
	if (interpolator == NULL || beforeImage.empty())
		return 0;
	
	// tạo 1 vector 3 dòng 1 cột chứa Px Py 1
	float B[] = { 0,0,1.0 };
	Mat P = Mat(3, 1, CV_32FC1, B);

	uchar* pSrc = beforeImage.data;
	Mat aff = transformer->getter(); 
	Mat ReverseAffine=aff.inv();
	for (int i = 0; i < afterImage.rows; i++) {
		uchar* pDes = afterImage.ptr<uchar>(i);
		for (int j = 0; j < afterImage.cols; j++) {
			P.ptr<float>(0)[0] = i;
			P.ptr<float>(1)[0] = j;

			// affine ngược
			Mat tPosition = ReverseAffine * P;

			float tx = tPosition.ptr<float>(0)[0],
			ty = tPosition.ptr<float>(1)[0];
			if (tx >= 0 && ty >= 0 && tx < beforeImage.rows && ty < beforeImage.cols)
				for (int c = 0; c < beforeImage.step[1]; c++) {
					pDes[j * beforeImage.step[1] + c] = interpolator->Interpolate(tx, ty, pSrc + c, beforeImage.step[0], beforeImage.step[1]);
				}
		}
	}
	return 0;
}
int GeometricTransformer::Scale(const Mat& srcImage, Mat& dstImage, float sx, float sy, PixelInterpolate* interpolator) {
	if (interpolator == NULL || srcImage.empty())
		return 0;
	AffineTransform* AT = new AffineTransform();
	AT->Translate(-srcImage.rows / 2, -srcImage.cols / 2);
	AT->Scale(sx, sy);
	AT->Translate(srcImage.rows / 2, srcImage.cols / 2);
	dstImage = Mat(srcImage.rows, srcImage.cols, CV_8UC3, Scalar(0));
	return this->Transform(srcImage, dstImage, AT, interpolator);
}
int GeometricTransformer::Resize(const Mat& srcImage, Mat& dstImage, int newWidth, int newHeight, PixelInterpolate* interpolator) {
	if (interpolator == NULL || srcImage.empty())
		return 0;
	dstImage = Mat(newHeight, newWidth, CV_8UC3, Scalar(0));
	AffineTransform* AT = new AffineTransform();
	float sx = (1.0 * newHeight) / (1.0 * srcImage.rows);
	float sy = (1.0 * newWidth) / (1.0 * srcImage.cols);
	AT->Scale(sx, sy);
	return this->Transform(srcImage, dstImage, AT, interpolator);
}
int GeometricTransformer::Flip(const Mat& srcImage, Mat& dstImage, bool direction, PixelInterpolate* interpolator)	 {
	//Không thể lật ảnh
	if (interpolator == NULL || srcImage.empty())
		return 0;
	AffineTransform* AT = new AffineTransform();
	sizeC = srcImage.cols;
	sizeR = srcImage.rows;
	if (direction) {
		AT->Scale(-1, 1);
		AT->Translate(srcImage.rows, 0);
	}
	else {
		AT->Scale(1, -1);
		AT->Translate(0, srcImage.cols);
	}
	dstImage = Mat(srcImage.rows, srcImage.cols, CV_8UC3, Scalar(0));

	// thực hiện biến đổi affine thuận và nghịch, nếu không biến đổi thành công thì chekc  = 0
	return this->Transform(srcImage, dstImage, AT, interpolator);
}
int GeometricTransformer::RotateUnkeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator) {
	if (interpolator == NULL || srcImage.empty())
		return 0;
	AffineTransform* AT = new AffineTransform();
	AT->Translate(-srcImage.rows / 2, -srcImage.cols / 2);
	AT->Rotate(angle);
	AT->Translate(srcImage.rows / 2, srcImage.cols / 2);
	dstImage = Mat(srcImage.rows, srcImage.cols, CV_8UC3, Scalar(0));
	return this->Transform(srcImage, dstImage, AT, interpolator);
}
int GeometricTransformer::RotateKeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator) {
	//Các thông số widthStep, nChannels để nội suy màu
	int rows = srcImage.rows;
	int cols = srcImage.cols;

	int widthStep = srcImage.step[0];
	int nChannels = srcImage.step[1];
	//Đổi góc từ độ sang radian
	//float theta = angle * _PI / 180;

	AffineTransform* AT = new AffineTransform();
	//Đưa tâm ảnh về gốc tọa độ của ảnh gốc
	AT->Translate(-rows / 2, -cols / 2);
	//Xoay ảnh
	AT->Rotate(angle);

	float X[] = { 0, 0, srcImage.rows, srcImage.rows };
	float Y[] = { 0, srcImage.cols, 0, srcImage.cols };
	//Tiến hành xoay các biến đổi hình học các đỉnh của ảnh
	for (int i = 0; i < 4; i++) {
		AT->TransformPoint(X[i], Y[i]);
	}
	//Lấy 4 điểm xa nhất theo 4 hướng
	float minx = std::min({ X[0], X[1], X[2], X[3] });
	float maxx = std::max({ X[0], X[1], X[2], X[3] });
	float miny = std::min({ Y[0], Y[1], Y[2], Y[3] });
	float maxy = std::max({ Y[0], Y[1], Y[2], Y[3] });
	//Kích thước ảnh mới
	int newRows = (int)floor(maxx - minx);
	int newCols = (int)floor(maxy - miny);
	//Đưa tâm ảnh về chính giữa ảnh mới
	AT->Translate(newRows / 2, newCols / 2);
	dstImage = Mat(newRows, newCols, CV_8UC3, Scalar(0));
	return this->Transform(srcImage, dstImage, AT, interpolator);
}