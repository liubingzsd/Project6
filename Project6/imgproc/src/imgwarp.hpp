#ifndef OPENCV_IMGPROC_IMGWARP_HPP
#define OPENCV_IMGPROC_IMGWARP_HPP
#include "precomp.hpp"

namespace cv
{
	namespace opt_AVX2
	{
#if CV_TRY_AVX2
		int warpAffineBlockline(int *adelta, int *bdelta, short* xy, short* alpha, int X0, int Y0, int bw);
#endif
	}

	namespace opt_SSE4_1
	{
#if CV_TRY_SSE4_1
		void convertMaps_nninterpolate32f1c16s_SSE41(const float* src1f, const float* src2f, short* dst1, int width);
		void convertMaps_32f1c16s_SSE41(const float* src1f, const float* src2f, short* dst1, ushort* dst2, int width);
		void convertMaps_32f2c16s_SSE41(const float* src1f, short* dst1, ushort* dst2, int width);
		void WarpAffineInvoker_Blockline_SSE41(int *adelta, int *bdelta, short* xy, int X0, int Y0, int bw);

		class WarpPerspectiveLine_SSE4
		{
		public:
			static Ptr<WarpPerspectiveLine_SSE4> getImpl(const double *M);
			virtual void processNN(const double *M, short* xy, double X0, double Y0, double W0, int bw) = 0;
			virtual void process(const double *M, short* xy, short* alpha, double X0, double Y0, double W0, int bw) = 0;
			virtual ~WarpPerspectiveLine_SSE4() {};
		};
#endif
	}
}
#endif
/* End of file. */

