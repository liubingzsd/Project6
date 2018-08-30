#ifndef __OPENCV_FEATURES_2D_FAST_HPP__
#define __OPENCV_FEATURES_2D_FAST_HPP__

#ifdef __cplusplus

#include "precomp.hpp"

namespace cv
{

	void makeOffsets(int pixel[25], int row_stride, int patternSize);

	template<int patternSize>
	int cornerScore(const uchar* ptr, const int pixel[], int threshold);

}

#endif
#endif
