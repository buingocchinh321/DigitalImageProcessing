#include "Libraries.h"
using namespace cv;
using namespace std;

int main(int argc, char* argv[]) {
	string op, in, out;
	if (argc == 4) {
		op = argv[1];
		if (op == "-rgb2gray") {
			in = argv[2];
			out = argv[3];

			Mat srcImg = imread(in);
			Mat outImg;
			Converter cvter;
			cvter.Convert(srcImg, outImg, 0);
			imwrite(out, outImg);
		}
		else if (op == "-rgb2hsv") {
			in = argv[2];
			out = argv[3];

			Mat srcImg = imread(in);
			Mat outImg;
			Converter cvter;
			cvter.Convert(srcImg, outImg, 1);
			imwrite(out, outImg);
		}
		else if (op == "-drawhist") {
			in = argv[2];
			out = argv[3];

			Mat srcImg = imread(in);
			Mat outImg;
			ColorTransformer ct;
			Mat red, green, blue, hist;
			vector<Mat> bgr;
			split(srcImg, bgr);
			int hsize = 256;
			float range[] = { 0, 256 };  // the upper boundary is exclusive
			const float* histranges = range;
			calcHist(&bgr[0], 1, 0, Mat(), blue, 1, &hsize, &histranges);
			calcHist(&bgr[1], 1, 0, Mat(), green, 1, &hsize, &histranges);
			calcHist(&bgr[2], 1, 0, Mat(), red, 1, &hsize, &histranges);
			bgr = { blue,green,red };
			merge(bgr, hist);
			ct.DrawHistogram(hist, outImg);
			imwrite(out, outImg);
		}
		else if (op == "-equalhist") {
			in = argv[2];
			out = argv[3];

			Mat srcImg = imread(in);
			Mat outImg;
			ColorTransformer ct;
			Mat red, green, blue, hist;
			vector<Mat> bgr;
			split(srcImg, bgr);
			ct.HistogramEqualization(bgr[0], bgr[0]);
			ct.HistogramEqualization(bgr[1], bgr[1]);
			ct.HistogramEqualization(bgr[2], bgr[2]);
			merge(bgr, outImg);
			imwrite(out, outImg);
		}
		else if (op == "-sobel") {
			in = argv[2];
			out = argv[3];

			Mat srcImg = imread(in, IMREAD_GRAYSCALE);
			Mat outImg;

			EdgeDetector ed;

			ed.DetectEdge(srcImg, outImg, 1);
			imwrite(out, outImg);
		}
		else if (op == "-prew") {
			in = argv[2];
			out = argv[3];

			Mat srcImg = imread(in, IMREAD_GRAYSCALE);
			Mat outImg;

			EdgeDetector ed;

			ed.DetectEdge(srcImg, outImg, 2);
			imwrite(out, outImg);
		}
		else if (op == "-lap") {
			in = argv[2];
			out = argv[3];

			Mat srcImg = imread(in, IMREAD_GRAYSCALE);
			Mat outImg;

			EdgeDetector ed;

			ed.DetectEdge(srcImg, outImg, 3);
			imwrite(out, outImg);
		}
	}
	else if (argc == 5) {
		op = argv[1];
		if (op == "-bright") {
			in = argv[3];
			out = argv[4];
			float a = stof(argv[2]);
			Mat srcImg = imread(in);
			Mat outImg;
			ColorTransformer ct;
			ct.ChangeBrighness(srcImg, outImg, (short)a);
			imwrite(out, outImg);
		}
		else if (op == "-contrast") {
			in = argv[3];
			out = argv[4];
			float a = stof(argv[2]);
			Mat srcImg = imread(in);
			Mat outImg;
			ColorTransformer ct;
			ct.ChangeContrast(srcImg, outImg, a);
			imwrite(out, outImg);
		}
		else if (op == "-flipH") {
			string inter = argv[2];
			in = argv[3];
			out = argv[4];
			PixelInterpolate* PI = nullptr;
			if (inter == "-bl") PI = new BilinearInterpolate();
			else if (inter == "-nn") PI = new NearestNeighborInterpolate();
			else return 0;
			Mat src = imread(in), outI;
			GeometricTransformer Geo;
			Geo.Flip(src, outI, 1, PI);
			imwrite(out, outI);
		}
		else if (op == "-flipV") {
			string inter = argv[2];
			in = argv[3];
			out = argv[4];
			PixelInterpolate* PI = nullptr;
			if (inter == "-bl") PI = new BilinearInterpolate();
			else if (inter == "-nn") PI = new NearestNeighborInterpolate();
			else return 0;
			Mat src = imread(in), outI;
			GeometricTransformer Geo;
			Geo.Flip(src, outI, 0, PI);
			imwrite(out, outI);
		}
	}
	else if (argc == 6) {
		op = argv[1];

		if (op == "-rotK") {
			string inter = argv[2];
			float a = stof(argv[3]);
			in = argv[4];
			out = argv[5];
			PixelInterpolate* PI = nullptr;
			if (inter == "-bl") PI = new BilinearInterpolate();
			else if (inter == "-nn") PI = new NearestNeighborInterpolate();
			else return 0;
			Mat src = imread(in), outI;
			GeometricTransformer Geo;
			Geo.RotateUnkeepImage(src, outI, a, PI);
			imwrite(out, outI);
		}
		else if (op == "-rotP") {
			string inter = argv[2];
			float a = stof(argv[3]);
			in = argv[4];
			out = argv[5];
			PixelInterpolate* PI = nullptr;
			if (inter == "-bl") PI = new BilinearInterpolate();
			else if (inter == "-nn") PI = new NearestNeighborInterpolate();
			else return 0;
			Mat src = imread(in), outI;
			GeometricTransformer Geo;
			Geo.RotateKeepImage(src, outI, a, PI);
			imwrite(out, outI);
		}
		else if (op == "-avg") {
			in = argv[4];
			out = argv[5];
			int kx = stoi(argv[2]), ky = stoi(argv[3]);
			Mat src = imread(in,IMREAD_GRAYSCALE), outI;
			Blur b;

			if (kx % 2 == 0 || ky % 2 == 0) return 1;
			b.BlurImage(src,outI,kx,ky,0);

			imwrite(out, outI);
		}
		else if (op == "-med") {
			in = argv[4];
			out = argv[5];
			int kx = stoi(argv[2]), ky = stoi(argv[3]);
			Mat src = imread(in, IMREAD_GRAYSCALE), outI;
			Blur b;

			if (kx % 2 == 0 || ky % 2 == 0) return 1;
			b.BlurImage(src, outI, kx, ky, 1);

			imwrite(out, outI);
		}
		else if (op == "-gau") {
			in = argv[4];
			out = argv[5];
			int kx = stoi(argv[2]), ky = stoi(argv[3]);
			Mat src = imread(in, IMREAD_GRAYSCALE), outI;
			Blur b;

			if (kx % 2 == 0 || ky % 2 == 0) return 1;
			b.BlurImage(src, outI, kx, ky, 2);

			imwrite(out, outI);
		}
	}
	else if (argc == 7) {
		op = argv[1];

		if (op == "-zoom") {
			string inter = argv[2];
			float a = stof(argv[3]),
				b = stof(argv[4]);
			in = argv[5];
			out = argv[6];
			PixelInterpolate* PI = nullptr;
			if (inter == "-bl") PI = new BilinearInterpolate();
			else if (inter == "-nn") PI = new NearestNeighborInterpolate();
			else return 0;
			Mat src = imread(in), outI;
			GeometricTransformer Geo;
			Geo.Scale(src, outI, a, b, PI);
			imwrite(out, outI);
		}
		else if (op == "-resize") {
			string inter = argv[2];
			float a = stof(argv[3]),
				b = stof(argv[4]);
			in = argv[5];
			out = argv[6];
			PixelInterpolate* PI = nullptr;
			if (inter == "-bl") PI = new BilinearInterpolate();
			else if (inter == "-nn") PI = new NearestNeighborInterpolate();
			else return 0;
			Mat src = imread(in), outI;
			GeometricTransformer Geo;
			Geo.Resize(src, outI, a, b, PI);
			imwrite(out, outI);
		}
		

	}
}

//int main() {
//	PixelInterpolate* PI = new BilinearInterpolate();
//	Mat src = imread("shiba.jpg"), outI;
//	GeometricTransformer Geo;
//	Geo.Flip(src.clone(), outI, 0, PI);
//	imwrite("D:/scale.jpg", outI);
//}