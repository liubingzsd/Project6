#include "precomp.hpp"

void cv::cornerSubPix(InputArray _image, InputOutputArray _corners,
	Size win, Size zeroZone, TermCriteria criteria)
{
	CV_INSTRUMENT_REGION()

	const int MAX_ITERS = 100;
	int win_w = win.width * 2 + 1, win_h = win.height * 2 + 1;
	int i, j, k;
	int max_iters = (criteria.type & CV_TERMCRIT_ITER) ? MIN(MAX(criteria.maxCount, 1), MAX_ITERS) : MAX_ITERS;
	double eps = (criteria.type & CV_TERMCRIT_EPS) ? MAX(criteria.epsilon, 0.) : 0;
	eps *= eps; // use square of error in comparison operations

	cv::Mat src = _image.getMat(), cornersmat = _corners.getMat();
	int count = cornersmat.checkVector(2, CV_32F);
	CV_Assert(count >= 0);
	Point2f* corners = cornersmat.ptr<Point2f>();

	if (count == 0)
		return;

	CV_Assert(win.width > 0 && win.height > 0);
	CV_Assert(src.cols >= win.width * 2 + 5 && src.rows >= win.height * 2 + 5);
	CV_Assert(src.channels() == 1);

	Mat maskm(win_h, win_w, CV_32F), subpix_buf(win_h + 2, win_w + 2, CV_32F);
	float* mask = maskm.ptr<float>();

	for (i = 0; i < win_h; i++)
	{
		float y = (float)(i - win.height) / win.height;
		float vy = std::exp(-y * y);
		for (j = 0; j < win_w; j++)
		{
			float x = (float)(j - win.width) / win.width;
			mask[i * win_w + j] = (float)(vy*std::exp(-x * x));
		}
	}

	// make zero_zone
	if (zeroZone.width >= 0 && zeroZone.height >= 0 &&
		zeroZone.width * 2 + 1 < win_w && zeroZone.height * 2 + 1 < win_h)
	{
		for (i = win.height - zeroZone.height; i <= win.height + zeroZone.height; i++)
		{
			for (j = win.width - zeroZone.width; j <= win.width + zeroZone.width; j++)
			{
				mask[i * win_w + j] = 0;
			}
		}
	}

	// do optimization loop for all the points
	for (int pt_i = 0; pt_i < count; pt_i++)
	{
		Point2f cT = corners[pt_i], cI = cT;
		int iter = 0;
		double err = 0;

		do
		{
			Point2f cI2;
			double a = 0, b = 0, c = 0, bb1 = 0, bb2 = 0;

			getRectSubPix(src, Size(win_w + 2, win_h + 2), cI, subpix_buf, subpix_buf.type());
			const float* subpix = &subpix_buf.at<float>(1, 1);

			// process gradient
			for (i = 0, k = 0; i < win_h; i++, subpix += win_w + 2)
			{
				double py = i - win.height;

				for (j = 0; j < win_w; j++, k++)
				{
					double m = mask[k];
					double tgx = subpix[j + 1] - subpix[j - 1];
					double tgy = subpix[j + win_w + 2] - subpix[j - win_w - 2];
					double gxx = tgx * tgx * m;
					double gxy = tgx * tgy * m;
					double gyy = tgy * tgy * m;
					double px = j - win.width;

					a += gxx;
					b += gxy;
					c += gyy;

					bb1 += gxx * px + gxy * py;
					bb2 += gxy * px + gyy * py;
				}
			}

			double det = a * c - b * b;
			if (fabs(det) <= DBL_EPSILON * DBL_EPSILON)
				break;

			// 2x2 matrix inversion
			double scale = 1.0 / det;
			cI2.x = (float)(cI.x + c * scale*bb1 - b * scale*bb2);
			cI2.y = (float)(cI.y - b * scale*bb1 + a * scale*bb2);
			err = (cI2.x - cI.x) * (cI2.x - cI.x) + (cI2.y - cI.y) * (cI2.y - cI.y);
			cI = cI2;
			if (cI.x < 0 || cI.x >= src.cols || cI.y < 0 || cI.y >= src.rows)
				break;
		} while (++iter < max_iters && err > eps);

		// if new point is too far from initial, it means poor convergence.
		// leave initial point as the result
		if (fabs(cI.x - cT.x) > win.width || fabs(cI.y - cT.y) > win.height)
			cI = cT;

		corners[pt_i] = cI;
	}
}


CV_IMPL void
cvFindCornerSubPix(const void* srcarr, CvPoint2D32f* _corners,
	int count, CvSize win, CvSize zeroZone,
	CvTermCriteria criteria)
{
	if (!_corners || count <= 0)
		return;

	cv::Mat src = cv::cvarrToMat(srcarr), corners(count, 1, CV_32FC2, _corners);
	cv::cornerSubPix(src, corners, win, zeroZone, criteria);
}

/* End of file. */
