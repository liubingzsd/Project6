#include "precomp.hpp"

namespace cv
{
	static void getScharrKernels(OutputArray _kx, OutputArray _ky,
		int dx, int dy, bool normalize, int ktype)
	{
		const int ksize = 3;

		_kx.create(ksize, 1, ktype, -1, true);
		_ky.create(ksize, 1, ktype, -1, true);
		Mat kx = _kx.getMat();
		Mat ky = _ky.getMat();


		for (int k = 0; k < 2; k++)
		{
			Mat* kernel = k == 0 ? &kx : &ky;
			int order = k == 0 ? dx : dy;
			int kerI[3];

			if (order == 0)
				kerI[0] = 3, kerI[1] = 10, kerI[2] = 3;
			else if (order == 1)
				kerI[0] = -1, kerI[1] = 0, kerI[2] = 1;

			Mat temp(kernel->rows, kernel->cols, CV_32S, &kerI[0]);
			double scale = !normalize || order == 1 ? 1. : 1. / 32;
			temp.convertTo(*kernel, ktype, scale);
		}
	}

	static void getSobelKernels(OutputArray _kx, OutputArray _ky,
		int dx, int dy, int _ksize, bool normalize, int ktype)
	{
		int i, j, ksizeX = _ksize, ksizeY = _ksize;
		if (ksizeX == 1 && dx > 0)
			ksizeX = 3;
		if (ksizeY == 1 && dy > 0)
			ksizeY = 3;

		_kx.create(ksizeX, 1, ktype, -1, true);
		_ky.create(ksizeY, 1, ktype, -1, true);
		Mat kx = _kx.getMat();
		Mat ky = _ky.getMat();

		if (_ksize % 2 == 0 || _ksize > 31)
			CV_Error(CV_StsOutOfRange, "The kernel size must be odd and not larger than 31");
		std::vector<int> kerI(std::max(ksizeX, ksizeY) + 1);

		for (int k = 0; k < 2; k++)
		{
			Mat* kernel = k == 0 ? &kx : &ky;
			int order = k == 0 ? dx : dy;
			int ksize = k == 0 ? ksizeX : ksizeY;

			if (ksize == 1)
				kerI[0] = 1;
			else if (ksize == 3)
			{
				if (order == 0)
					kerI[0] = 1, kerI[1] = 2, kerI[2] = 1;
				else if (order == 1)
					kerI[0] = -1, kerI[1] = 0, kerI[2] = 1;
				else
					kerI[0] = 1, kerI[1] = -2, kerI[2] = 1;
			}
			else
			{
				int oldval, newval;
				kerI[0] = 1;
				for (i = 0; i < ksize; i++)
					kerI[i + 1] = 0;

				for (i = 0; i < ksize - order - 1; i++)
				{
					oldval = kerI[0];
					for (j = 1; j <= ksize; j++)
					{
						newval = kerI[j] + kerI[j - 1];
						kerI[j - 1] = oldval;
						oldval = newval;
					}
				}

				for (i = 0; i < order; i++)
				{
					oldval = -kerI[0];
					for (j = 1; j <= ksize; j++)
					{
						newval = kerI[j - 1] - kerI[j];
						kerI[j - 1] = oldval;
						oldval = newval;
					}
				}
			}

			Mat temp(kernel->rows, kernel->cols, CV_32S, &kerI[0]);
			double scale = !normalize ? 1. : 1. / (1 << (ksize - order - 1));
			temp.convertTo(*kernel, ktype, scale);
		}
	}

}

void cv::getDerivKernels(OutputArray kx, OutputArray ky, int dx, int dy,
	int ksize, bool normalize, int ktype)
{
	if (ksize <= 0)
		getScharrKernels(kx, ky, dx, dy, normalize, ktype);
	else
		getSobelKernels(kx, ky, dx, dy, ksize, normalize, ktype);
}

void cv::Sobel(InputArray _src, OutputArray _dst, int ddepth, int dx, int dy,
	int ksize, double scale, double delta, int borderType)
{
	
	int stype = _src.type(), sdepth = CV_MAT_DEPTH(stype), cn = CV_MAT_CN(stype);
	if (ddepth < 0)
		ddepth = sdepth;
	int dtype = CV_MAKE_TYPE(ddepth, cn);
	_dst.create(_src.size(), dtype);

	int ktype = std::max(CV_32F, std::max(ddepth, sdepth));

	Mat kx, ky;
	getDerivKernels(kx, ky, dx, dy, ksize, false, ktype);
	if (scale != 1)
	{
		// usually the smoothing part is the slowest to compute,
		// so try to scale it instead of the faster differentiating part
		if (dx == 0)
			kx *= scale;
		else
			ky *= scale;
	}

	
	Mat src = _src.getMat();
	Mat dst = _dst.getMat();

	Point ofs;
	Size wsz(src.cols, src.rows);
	if (!(borderType & BORDER_ISOLATED))
		src.locateROI(wsz, ofs);
	sepFilter2D(src, dst, ddepth, kx, ky, Point(-1, -1), delta, borderType);
}


void cv::Scharr(InputArray _src, OutputArray _dst, int ddepth, int dx, int dy,
	double scale, double delta, int borderType)
{

	int stype = _src.type(), sdepth = CV_MAT_DEPTH(stype), cn = CV_MAT_CN(stype);
	if (ddepth < 0)
		ddepth = sdepth;
	int dtype = CV_MAKETYPE(ddepth, cn);
	_dst.create(_src.size(), dtype);

	int ktype = std::max(CV_32F, std::max(ddepth, sdepth));

	Mat kx, ky;
	getScharrKernels(kx, ky, dx, dy, false, ktype);
	if (scale != 1)
	{
		// usually the smoothing part is the slowest to compute,
		// so try to scale it instead of the faster differentiating part
		if (dx == 0)
			kx *= scale;
		else
			ky *= scale;
	}

	Mat src = _src.getMat();
	Mat dst = _dst.getMat();

	Point ofs;
	Size wsz(src.cols, src.rows);
	if (!(borderType & BORDER_ISOLATED))
		src.locateROI(wsz, ofs);
	sepFilter2D(src, dst, ddepth, kx, ky, Point(-1, -1), delta, borderType);
}
