#ifndef __VIDEOIO_H_
#define __VIDEOIO_H_

#include "../include/opencv2/videoio.hpp"

#include "../../core/include/opencv2/core/utility.hpp"
#include "../../core/include/opencv2/core/private.hpp"

#include "../../imgcodecs/include/opencv2/imgcodecs.hpp"

#include "../../imgproc/include/opencv2/imgproc.hpp"

#include "../../imgproc/include/opencv2/imgproc/imgproc_c.h"
#include "../../imgcodecs/include/opencv2/imgcodecs/imgcodecs_c.h"
#include "../include/opencv2/videoio/videoio_c.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>

#if defined _WIN32 || defined WINCE
#if !defined _WIN32_WINNT
#ifdef HAVE_MSMF
#define _WIN32_WINNT 0x0600 // Windows Vista
#else
#define _WIN32_WINNT 0x0501 // Windows XP
#endif
#endif

#include <windows.h>
#undef small
#undef min
#undef max
#undef abs
#endif

#define __BEGIN__ __CV_BEGIN__
#define __END__  __CV_END__
#define EXIT __CV_EXIT__

/***************************** CvCapture structure ******************************/

struct CvCapture
{
	virtual ~CvCapture() {}
	virtual double getProperty(int) const { return 0; }
	virtual bool setProperty(int, double) { return 0; }
	virtual bool grabFrame() { return true; }
	virtual IplImage* retrieveFrame(int) { return 0; }
	virtual int getCaptureDomain() { return cv::CAP_ANY; } // Return the type of the capture object: CAP_VFW, etc...
};

/*************************** CvVideoWriter structure ****************************/

struct CvVideoWriter
{
	virtual ~CvVideoWriter() {}
	virtual bool writeFrame(const IplImage*) { return false; }
};

CvCapture * cvCreateCameraCapture_V4L(int index);
CvCapture * cvCreateCameraCapture_V4L(const char* deviceName);
CvCapture * cvCreateCameraCapture_DC1394(int index);
CvCapture * cvCreateCameraCapture_DC1394_2(int index);
CvCapture* cvCreateCameraCapture_MIL(int index);
CvCapture* cvCreateCameraCapture_Giganetix(int index);
CvCapture * cvCreateCameraCapture_CMU(int index);
CvCapture* cvCreateFileCapture_Win32(const char* filename);
CvCapture* cvCreateCameraCapture_VFW(int index);
CvCapture* cvCreateFileCapture_VFW(const char* filename);
CvVideoWriter* cvCreateVideoWriter_Win32(const char* filename, int fourcc,
	double fps, CvSize frameSize, int is_color);
CvVideoWriter* cvCreateVideoWriter_VFW(const char* filename, int fourcc,
	double fps, CvSize frameSize, int is_color);
CvCapture* cvCreateCameraCapture_DShow(int index);
CvCapture* cvCreateCameraCapture_MSMF(int index);
CvCapture* cvCreateFileCapture_MSMF(const char* filename);
CvVideoWriter* cvCreateVideoWriter_MSMF(const char* filename, int fourcc,
	double fps, CvSize frameSize, int is_color);
CvCapture* cvCreateCameraCapture_OpenNI(int index);
CvCapture* cvCreateCameraCapture_OpenNI2(int index);
CvCapture* cvCreateFileCapture_OpenNI(const char* filename);
CvCapture* cvCreateFileCapture_OpenNI2(const char* filename);
CvCapture* cvCreateCameraCapture_Android(int index);
CvCapture* cvCreateCameraCapture_XIMEA(int index);
CvCapture* cvCreateCameraCapture_XIMEA(const char* serialNumber);
CvCapture* cvCreateCameraCapture_AVFoundation(int index);
CvCapture* cvCreateCameraCapture_Aravis(int index);

CvCapture* cvCreateFileCapture_Images(const char* filename);
CvVideoWriter* cvCreateVideoWriter_Images(const char* filename);

CvCapture* cvCreateFileCapture_XINE(const char* filename);


#define CV_CAP_GSTREAMER_1394		0
#define CV_CAP_GSTREAMER_V4L		1
#define CV_CAP_GSTREAMER_V4L2		2
#define CV_CAP_GSTREAMER_FILE		3

CvCapture* cvCreateCapture_GStreamer(int type, const char *filename);
CvCapture* cvCreateFileCapture_FFMPEG_proxy(const char* filename);


CvVideoWriter* cvCreateVideoWriter_FFMPEG_proxy(const char* filename, int fourcc,
	double fps, CvSize frameSize, int is_color);

CvCapture * cvCreateFileCapture_QT(const char  * filename);
CvCapture * cvCreateCameraCapture_QT(const int     index);

CvVideoWriter* cvCreateVideoWriter_QT(const char* filename, int fourcc,
	double fps, CvSize frameSize, int is_color);

CvCapture* cvCreateFileCapture_AVFoundation(const char * filename);
CvVideoWriter* cvCreateVideoWriter_AVFoundation(const char* filename, int fourcc,
	double fps, CvSize frameSize, int is_color);


CvCapture * cvCreateCameraCapture_Unicap(const int     index);
CvCapture * cvCreateCameraCapture_PvAPI(const int     index);
CvVideoWriter* cvCreateVideoWriter_GStreamer(const char* filename, int fourcc,
	double fps, CvSize frameSize, int is_color);


namespace cv
{
	class IVideoCapture
	{
	public:
		virtual ~IVideoCapture() {}
		virtual double getProperty(int) const { return 0; }
		virtual bool setProperty(int, double) { return false; }
		virtual bool grabFrame() = 0;
		virtual bool retrieveFrame(int, OutputArray) = 0;
		virtual bool isOpened() const = 0;
		virtual int getCaptureDomain() { return CAP_ANY; } // Return the type of the capture object: CAP_VFW, etc...
	};

	class IVideoWriter
	{
	public:
		virtual ~IVideoWriter() {}
		virtual double getProperty(int) const { return 0; }
		virtual bool setProperty(int, double) { return false; }

		virtual bool isOpened() const = 0;
		virtual void write(InputArray) = 0;
	};

	Ptr<IVideoCapture> createMotionJpegCapture(const String& filename);
	Ptr<IVideoWriter> createMotionJpegWriter(const String& filename, double fps, Size frameSize, bool iscolor);

	Ptr<IVideoCapture> createGPhoto2Capture(int index);
	Ptr<IVideoCapture> createGPhoto2Capture(const String& deviceName);
}

#endif /* __VIDEOIO_H_ */

