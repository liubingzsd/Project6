#include "precomp.hpp"
#include "stat.simd.hpp"
//#include "stat.simd_declarations.hpp" // defines CV_CPU_DISPATCH_MODES_ALL=AVX2,...,BASELINE based on CMakeLists.txt content

namespace cv {
	namespace hal {

		int normHamming(const uchar* a, int n)
		{
			CV_INSTRUMENT_REGION()
				return 0;
		}

		int normHamming(const uchar* a, const uchar* b, int n)
		{
			CV_INSTRUMENT_REGION()
				return 0;
		}

	}
} //cv::hal
