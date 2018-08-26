#ifndef OPENCV_IMGPROC_HAL_REPLACEMENT_HPP
#define OPENCV_IMGPROC_HAL_REPLACEMENT_HPP

#include "../../core/include/opencv2/hal/interface.h"
#include "../include/opencv2/imgproc/hal/interface.h"

struct cvhalFilter2D {};
inline int hal_ni_filterInit(cvhalFilter2D **context, uchar *kernel_data, size_t kernel_step, int kernel_type, int kernel_width, int kernel_height, int max_width, int max_height, int src_type, int dst_type, int borderType, double delta, int anchor_x, int anchor_y, bool allowSubmatrix, bool allowInplace) { return CV_HAL_ERROR_NOT_IMPLEMENTED; }
inline int hal_ni_filter(cvhalFilter2D *context, uchar *src_data, size_t src_step, uchar *dst_data, size_t dst_step, int width, int height, int full_width, int full_height, int offset_x, int offset_y) { return CV_HAL_ERROR_NOT_IMPLEMENTED; }
inline int hal_ni_filterFree(cvhalFilter2D *context) { return CV_HAL_ERROR_NOT_IMPLEMENTED; }
#define cv_hal_filterInit hal_ni_filterInit
#define cv_hal_filter hal_ni_filter
#define cv_hal_filterFree hal_ni_filterFree


#endif
