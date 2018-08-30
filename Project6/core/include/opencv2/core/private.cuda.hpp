#ifndef OPENCV_CORE_PRIVATE_CUDA_HPP
#define OPENCV_CORE_PRIVATE_CUDA_HPP


#include "cvdef.h"
#include "base.hpp"

#include "cuda.hpp"

#ifdef HAVE_CUDA
#  include <cuda.h>
#  include <cuda_runtime.h>
#  if defined(__CUDACC_VER_MAJOR__) && (8 <= __CUDACC_VER_MAJOR__)
#    if defined (__GNUC__) && !defined(__CUDACC__)
#     pragma GCC diagnostic push
#     pragma GCC diagnostic ignored "-Wstrict-aliasing"
#     include <cuda_fp16.h>
#     pragma GCC diagnostic pop
#    else
#     include <cuda_fp16.h>
#    endif
#  endif // defined(__CUDACC_VER_MAJOR__) && (8 <= __CUDACC_VER_MAJOR__)
#  include <npp.h>
#  include "opencv2/core/cuda_stream_accessor.hpp"
#  include "opencv2/core/cuda/common.hpp"

#  define NPP_VERSION (NPP_VERSION_MAJOR * 1000 + NPP_VERSION_MINOR * 100 + NPP_VERSION_BUILD)

#  define CUDART_MINIMUM_REQUIRED_VERSION 6050

#  if (CUDART_VERSION < CUDART_MINIMUM_REQUIRED_VERSION)
#    error "Insufficient Cuda Runtime library version, please update it."
#  endif

#  if defined(CUDA_ARCH_BIN_OR_PTX_10)
#    error "OpenCV CUDA module doesn't support NVIDIA compute capability 1.0"
#  endif
#endif

//! @cond IGNORED

namespace cv {
	namespace cuda {
		CV_EXPORTS cv::String getNppErrorMessage(int code);
		CV_EXPORTS cv::String getCudaDriverApiErrorMessage(int code);

		CV_EXPORTS GpuMat getInputMat(InputArray _src, Stream& stream);

		CV_EXPORTS GpuMat getOutputMat(OutputArray _dst, int rows, int cols, int type, Stream& stream);
		static inline GpuMat getOutputMat(OutputArray _dst, Size size, int type, Stream& stream)
		{
			return getOutputMat(_dst, size.height, size.width, type, stream);
		}

		CV_EXPORTS void syncOutput(const GpuMat& dst, OutputArray _dst, Stream& stream);
	}
}

#ifndef HAVE_CUDA

static inline void throw_no_cuda() { CV_Error(cv::Error::GpuNotSupported, "The library is compiled without CUDA support"); }

#else // HAVE_CUDA

static inline void throw_no_cuda() { CV_Error(cv::Error::StsNotImplemented, "The called functionality is disabled for current build or platform"); }

namespace cv {
	namespace cuda
	{
		static inline void checkNppError(int code, const char* file, const int line, const char* func)
		{
			if (code < 0)
				cv::error(cv::Error::GpuApiCallError, getNppErrorMessage(code), func, file, line);
		}

		static inline void checkCudaDriverApiError(int code, const char* file, const int line, const char* func)
		{
			if (code != CUDA_SUCCESS)
				cv::error(cv::Error::GpuApiCallError, getCudaDriverApiErrorMessage(code), func, file, line);
		}

		template<int n> struct NPPTypeTraits;
		template<> struct NPPTypeTraits<CV_8U> { typedef Npp8u npp_type; };
		template<> struct NPPTypeTraits<CV_8S> { typedef Npp8s npp_type; };
		template<> struct NPPTypeTraits<CV_16U> { typedef Npp16u npp_type; };
		template<> struct NPPTypeTraits<CV_16S> { typedef Npp16s npp_type; };
		template<> struct NPPTypeTraits<CV_32S> { typedef Npp32s npp_type; };
		template<> struct NPPTypeTraits<CV_32F> { typedef Npp32f npp_type; };
		template<> struct NPPTypeTraits<CV_64F> { typedef Npp64f npp_type; };

		class NppStreamHandler
		{
		public:
			inline explicit NppStreamHandler(Stream& newStream)
			{
				oldStream = nppGetStream();
				nppSetStream(StreamAccessor::getStream(newStream));
			}

			inline explicit NppStreamHandler(cudaStream_t newStream)
			{
				oldStream = nppGetStream();
				nppSetStream(newStream);
			}

			inline ~NppStreamHandler()
			{
				nppSetStream(oldStream);
			}

		private:
			cudaStream_t oldStream;
		};
	}
}

#define nppSafeCall(expr)  cv::cuda::checkNppError(expr, __FILE__, __LINE__, CV_Func)
#define cuSafeCall(expr)  cv::cuda::checkCudaDriverApiError(expr, __FILE__, __LINE__, CV_Func)

#endif // HAVE_CUDA

//! @endcond

#endif // OPENCV_CORE_PRIVAT