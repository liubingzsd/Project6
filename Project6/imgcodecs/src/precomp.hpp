#ifndef __IMGCODECS_H_
#define __IMGCODECS_H_

#include "../include/opencv2/imgcodecs.hpp"

#include "../../core/include/opencv2/core/utility.hpp"
#include "../../core/include/opencv2/core/private.hpp"

#include "../../imgproc/include/opencv2/imgproc.hpp"
#include "../../imgproc/include/opencv2/imgproc/imgproc_c.h"
#include "../include/opencv2/imgcodecs/imgcodecs_c.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#if defined _WIN32 || defined WINCE
#include <windows.h>
#undef small
#undef min
#undef max
#undef abs
#endif

#define __BEGIN__ __CV_BEGIN__
#define __END__  __CV_END__
#define EXIT __CV_EXIT__

#endif /* __IMGCODECS_H_ */

