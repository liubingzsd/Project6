#include "precomp.hpp"

#include "mathfuncs_core.simd.hpp"
//#include "mathfuncs_core.simd_declarations.hpp" // defines CV_CPU_DISPATCH_MODES_ALL=AVX2,...,BASELINE based on CMakeLists.txt content
#if 0
namespace cv {
	namespace hal {

		///////////////////////////////////// ATAN2 ////////////////////////////////////

		void fastAtan32f(const float *Y, const float *X, float *angle, int len, bool angleInDegrees)
		{
			CV_INSTRUMENT_REGION()

			CALL_HAL(fastAtan32f, cv_hal_fastAtan32f, Y, X, angle, len, angleInDegrees);

		}

		void fastAtan64f(const double *Y, const double *X, double *angle, int len, bool angleInDegrees)
		{
			CV_INSTRUMENT_REGION()

			CALL_HAL(fastAtan64f, cv_hal_fastAtan64f, Y, X, angle, len, angleInDegrees);

		}

		// deprecated
		void fastAtan2(const float *Y, const float *X, float *angle, int len, bool angleInDegrees)
		{
			CV_INSTRUMENT_REGION()

			fastAtan32f(Y, X, angle, len, angleInDegrees);
		}

		void magnitude32f(const float* x, const float* y, float* mag, int len)
		{
			CV_INSTRUMENT_REGION()

			CALL_HAL(magnitude32f, cv_hal_magnitude32f, x, y, mag, len);
			// SSE42 performance issues
			CV_IPP_RUN(IPP_VERSION_X100 > 201800 || cv::ipp::getIppTopFeatures() != ippCPUID_SSE42, CV_INSTRUMENT_FUN_IPP(ippsMagnitude_32f, x, y, mag, len) >= 0);

		}

		void magnitude64f(const double* x, const double* y, double* mag, int len)
		{
			CV_INSTRUMENT_REGION()

				CALL_HAL(magnitude64f, cv_hal_magnitude64f, x, y, mag, len);
			// SSE42 performance issues
			CV_IPP_RUN(IPP_VERSION_X100 > 201800 || cv::ipp::getIppTopFeatures() != ippCPUID_SSE42, CV_INSTRUMENT_FUN_IPP(ippsMagnitude_64f, x, y, mag, len) >= 0);

		}


		void invSqrt32f(const float* src, float* dst, int len)
		{
			CV_INSTRUMENT_REGION()

			CALL_HAL(invSqrt32f, cv_hal_invSqrt32f, src, dst, len);
			CV_IPP_RUN_FAST(CV_INSTRUMENT_FUN_IPP(ippsInvSqrt_32f_A21, src, dst, len) >= 0);

		}


		void invSqrt64f(const double* src, double* dst, int len)
		{
			CV_INSTRUMENT_REGION()

			CALL_HAL(invSqrt64f, cv_hal_invSqrt64f, src, dst, len);
			CV_IPP_RUN_FAST(CV_INSTRUMENT_FUN_IPP(ippsInvSqrt_64f_A50, src, dst, len) >= 0);

		}


		void sqrt32f(const float* src, float* dst, int len)
		{
			CV_INSTRUMENT_REGION()

			CALL_HAL(sqrt32f, cv_hal_sqrt32f, src, dst, len);

		}


		void sqrt64f(const double* src, double* dst, int len)
		{
			CV_INSTRUMENT_REGION()

			CALL_HAL(sqrt64f, cv_hal_sqrt64f, src, dst, len);

		}

		void exp32f(const float *src, float *dst, int n)
		{
			CV_INSTRUMENT_REGION()

			CALL_HAL(exp32f, cv_hal_exp32f, src, dst, n);
			CV_IPP_RUN_FAST(CV_INSTRUMENT_FUN_IPP(ippsExp_32f_A21, src, dst, n) >= 0);
		}

		void exp64f(const double *src, double *dst, int n)
		{
			CV_INSTRUMENT_REGION()

			CALL_HAL(exp64f, cv_hal_exp64f, src, dst, n);
			CV_IPP_RUN_FAST(CV_INSTRUMENT_FUN_IPP(ippsExp_64f_A50, src, dst, n) >= 0);
		}

		void log32f(const float *src, float *dst, int n)
		{
			CV_INSTRUMENT_REGION()

			CALL_HAL(log32f, cv_hal_log32f, src, dst, n);
			CV_IPP_RUN_FAST(CV_INSTRUMENT_FUN_IPP(ippsLn_32f_A21, src, dst, n) >= 0);

		}

		void log64f(const double *src, double *dst, int n)
		{
			CV_INSTRUMENT_REGION()

			CALL_HAL(log64f, cv_hal_log64f, src, dst, n);
			CV_IPP_RUN_FAST(CV_INSTRUMENT_FUN_IPP(ippsLn_64f_A50, src, dst, n) >= 0);
		}

		//=============================================================================
		// for compatibility with 3.0

		void exp(const float* src, float* dst, int n)
		{
			exp32f(src, dst, n);
		}

		void exp(const double* src, double* dst, int n)
		{
			exp64f(src, dst, n);
		}

		void log(const float* src, float* dst, int n)
		{
			log32f(src, dst, n);
		}

		void log(const double* src, double* dst, int n)
		{
			log64f(src, dst, n);
		}

		void magnitude(const float* x, const float* y, float* dst, int n)
		{
			magnitude32f(x, y, dst, n);
		}

		void magnitude(const double* x, const double* y, double* dst, int n)
		{
			magnitude64f(x, y, dst, n);
		}

		void sqrt(const float* src, float* dst, int len)
		{
			sqrt32f(src, dst, len);
		}

		void sqrt(const double* src, double* dst, int len)
		{
			sqrt64f(src, dst, len);
		}

		void invSqrt(const float* src, float* dst, int len)
		{
			invSqrt32f(src, dst, len);
		}

		void invSqrt(const double* src, double* dst, int len)
		{
			invSqrt64f(src, dst, len);
		}

	}
} // namespace cv::hal::
#endif
float cv::fastAtan2(float y, float x)
{
	using namespace cv::hal;
//	CV_CPU_CALL_BASELINE(fastAtan2, (y, x));
	return 0;
}
