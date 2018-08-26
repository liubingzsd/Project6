#include "precomp.hpp"


namespace cv
{
	static void calcMinEigenVal(const Mat& _cov, Mat& _dst)
	{
		int i, j;
		Size size = _cov.size();
#if CV_SIMD128
		bool haveSimd = hasSIMD128();
#endif

		if (_cov.isContinuous() && _dst.isContinuous())
		{
			size.width *= size.height;
			size.height = 1;
		}

		for (i = 0; i < size.height; i++)
		{
			const float* cov = _cov.ptr<float>(i);
			float* dst = _dst.ptr<float>(i);
			j = 0;

#if CV_SIMD128
			if (haveSimd)
			{
				v_float32x4 half = v_setall_f32(0.5f);
				for (; j <= size.width - v_float32x4::nlanes; j += v_float32x4::nlanes)
				{
					v_float32x4 v_a, v_b, v_c, v_t;
					v_load_deinterleave(cov + j * 3, v_a, v_b, v_c);
					v_a *= half;
					v_c *= half;
					v_t = v_a - v_c;
					v_t = v_muladd(v_b, v_b, (v_t * v_t));
					v_store(dst + j, (v_a + v_c) - v_sqrt(v_t));
				}
			}
#endif // CV_SIMD128

			for (; j < size.width; j++)
			{
				float a = cov[j * 3] * 0.5f;
				float b = cov[j * 3 + 1];
				float c = cov[j * 3 + 2] * 0.5f;
				dst[j] = (float)((a + c) - std::sqrt((a - c)*(a - c) + b * b));
			}
		}
	}


	enum { MINEIGENVAL = 0, HARRIS = 1, EIGENVALSVECS = 2 };

	static void
	cornerEigenValsVecs(const Mat& src, Mat& eigenv, int block_size,
			int aperture_size, int op_type, double k = 0.,
			int borderType = BORDER_DEFAULT)
	{
#if CV_SIMD128
		bool haveSimd = hasSIMD128();
#endif

		int depth = src.depth();
		double scale = (double)(1 << ((aperture_size > 0 ? aperture_size : 3) - 1)) * block_size;
		if (aperture_size < 0)
			scale *= 2.0;
		if (depth == CV_8U)
			scale *= 255.0;
		scale = 1.0 / scale;

		Mat Dx, Dy;
		if (aperture_size > 0)
		{
			Sobel(src, Dx, CV_32F, 1, 0, aperture_size, scale, 0, borderType);
			Sobel(src, Dy, CV_32F, 0, 1, aperture_size, scale, 0, borderType);
		}
		else
		{
			Scharr(src, Dx, CV_32F, 1, 0, scale, 0, borderType);
			Scharr(src, Dy, CV_32F, 0, 1, scale, 0, borderType);
		}

		Size size = src.size();
		Mat cov(size, CV_32FC3);
		int i, j;

		for (i = 0; i < size.height; i++)
		{
			float* cov_data = cov.ptr<float>(i);
			const float* dxdata = Dx.ptr<float>(i);
			const float* dydata = Dy.ptr<float>(i);
			j = 0;

#if CV_SIMD128
			if (haveSimd)
			{
				for (; j <= size.width - v_float32x4::nlanes; j += v_float32x4::nlanes)
				{
					v_float32x4 v_dx = v_load(dxdata + j);
					v_float32x4 v_dy = v_load(dydata + j);

					v_float32x4 v_dst0, v_dst1, v_dst2;
					v_dst0 = v_dx * v_dx;
					v_dst1 = v_dx * v_dy;
					v_dst2 = v_dy * v_dy;

					v_store_interleave(cov_data + j * 3, v_dst0, v_dst1, v_dst2);
				}
			}
#endif // CV_SIMD128

			for (; j < size.width; j++)
			{
				float dx = dxdata[j];
				float dy = dydata[j];

				cov_data[j * 3] = dx * dx;
				cov_data[j * 3 + 1] = dx * dy;
				cov_data[j * 3 + 2] = dy * dy;
			}
		}

		boxFilter(cov, cov, cov.depth(), Size(block_size, block_size),
			Point(-1, -1), false, borderType);

		if (op_type == MINEIGENVAL)
			calcMinEigenVal(cov, eigenv);
		else if (op_type == HARRIS)
			calcHarris(cov, eigenv, k);
		else if (op_type == EIGENVALSVECS)
			calcEigenValsVecs(cov, eigenv);
	}


}

void cv::cornerHarris(InputArray _src, OutputArray _dst, int blockSize, int ksize, double k, int borderType)
{
	Mat src = _src.getMat();
	_dst.create(src.size(), CV_32FC1);
	Mat dst = _dst.getMat();
	cornerEigenValsVecs(src, dst, blockSize, ksize, HARRIS, k, borderType);
}

