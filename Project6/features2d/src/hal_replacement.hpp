#ifndef OPENCV_FEATURES2D_HAL_REPLACEMENT_HPP
#define OPENCV_FEATURES2D_HAL_REPLACEMENT_HPP

#include "../../core/include/opencv2/core/hal/interface.h"

#if defined __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#elif defined _MSC_VER
#  pragma warning( push )
#  pragma warning( disable: 4100 )
#endif

//! @addtogroup features2d_hal_interface
//! @note Define your functions to override default implementations:
//! @code
//! #undef hal_add8u
//! #define hal_add8u my_add8u
//! @endcode
//! @{
/**
@brief Detects corners using the FAST algorithm, returns mask.
@param src_data,src_step Source image
@param dst_data,dst_step Destination mask
@param width,height Source image dimensions
@param type FAST type
*/
inline int hal_ni_FAST_dense(const uchar* src_data, size_t src_step, uchar* dst_data, size_t dst_step, int width, int height, int type) { return CV_HAL_ERROR_NOT_IMPLEMENTED; }

//! @cond IGNORED
#define cv_hal_FAST_dense hal_ni_FAST_dense
//! @endcond

/**
@brief Non-maximum suppression for FAST_9_16.
@param src_data,src_step Source mask
@param dst_data,dst_step Destination mask after NMS
@param width,height Source mask dimensions
*/
inline int hal_ni_FAST_NMS(const uchar* src_data, size_t src_step, uchar* dst_data, size_t dst_step, int width, int height) { return CV_HAL_ERROR_NOT_IMPLEMENTED; }

//! @cond IGNORED
#define cv_hal_FAST_NMS hal_ni_FAST_NMS
//! @endcond

/**
@brief Detects corners using the FAST algorithm.
@param src_data,src_step Source image
@param width,height Source image dimensions
@param keypoints_data Pointer to keypoints
@param keypoints_count Count of keypoints
@param threshold Threshold for keypoint
@param nonmax_suppression Indicates if make nonmaxima suppression or not.
@param type FAST type
*/
inline int hal_ni_FAST(const uchar* src_data, size_t src_step, int width, int height, uchar* keypoints_data, size_t* keypoints_count, int threshold, bool nonmax_suppression, int type) { return CV_HAL_ERROR_NOT_IMPLEMENTED; }

//! @cond IGNORED
#define cv_hal_FAST hal_ni_FAST
//! @endcond

//! @}


#if defined __GNUC__
#  pragma GCC diagnostic pop
#elif defined _MSC_VER
#  pragma warning( pop )
#endif

//#include "custom_hal.hpp"

//! @cond IGNORED
#define CALL_HAL_RET(name, fun, retval, ...) \
    int res = __CV_EXPAND(fun(__VA_ARGS__, &retval)); \
    if (res == CV_HAL_ERROR_OK) \
        return retval; \
    else if (res != CV_HAL_ERROR_NOT_IMPLEMENTED) \
        CV_Error_(cv::Error::StsInternal, \
            ("HAL implementation " CVAUX_STR(name) " ==> " CVAUX_STR(fun) " returned %d (0x%08x)", res, res));


#define CALL_HAL(name, fun, ...) \
{                                           \
    int res = __CV_EXPAND(fun(__VA_ARGS__)); \
    if (res == CV_HAL_ERROR_OK) \
        return; \
    else if (res != CV_HAL_ERROR_NOT_IMPLEMENTED) \
        CV_Error_(cv::Error::StsInternal, \
            ("HAL implementation " CVAUX_STR(name) " ==> " CVAUX_STR(fun) " returned %d (0x%08x)", res, res)); \
}
//! @endcond

#endif

