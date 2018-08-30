#ifndef OPENCV_VIDEOIO_H
#define OPENCV_VIDEOIO_H

#include "../../../../core/include/opencv2/core/core_c.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	/**
	@addtogroup videoio_c
	@{
	*/

	/****************************************************************************************\
	*                         Working with Video Files and Cameras                           *
	\****************************************************************************************/

	/** @brief "black box" capture structure

	In C++ use cv::VideoCapture
	*/
	typedef struct CvCapture CvCapture;

	/** @brief start capturing frames from video file
	*/
	CVAPI(CvCapture*) cvCreateFileCapture(const char* filename);

	/** @brief start capturing frames from video file. allows specifying a preferred API to use
	*/
	CVAPI(CvCapture*) cvCreateFileCaptureWithPreference(const char* filename, int apiPreference);

	enum
	{
		CV_CAP_ANY = 0,     // autodetect

		CV_CAP_MIL = 100,   // MIL proprietary drivers

		CV_CAP_VFW = 200,   // platform native
		CV_CAP_V4L = 200,
		CV_CAP_V4L2 = 200,

		CV_CAP_FIREWARE = 300,   // IEEE 1394 drivers
		CV_CAP_FIREWIRE = 300,
		CV_CAP_IEEE1394 = 300,
		CV_CAP_DC1394 = 300,
		CV_CAP_CMU1394 = 300,

		CV_CAP_STEREO = 400,   // TYZX proprietary drivers
		CV_CAP_TYZX = 400,
		CV_TYZX_LEFT = 400,
		CV_TYZX_RIGHT = 401,
		CV_TYZX_COLOR = 402,
		CV_TYZX_Z = 403,

		CV_CAP_QT = 500,   // QuickTime

		CV_CAP_UNICAP = 600,   // Unicap drivers

		CV_CAP_DSHOW = 700,   // DirectShow (via videoInput)
		CV_CAP_MSMF = 1400,  // Microsoft Media Foundation (via videoInput)

		CV_CAP_PVAPI = 800,   // PvAPI, Prosilica GigE SDK

		CV_CAP_OPENNI = 900,   // OpenNI (for Kinect)
		CV_CAP_OPENNI_ASUS = 910,   // OpenNI (for Asus Xtion)

		CV_CAP_ANDROID = 1000,  // Android - not used
		CV_CAP_ANDROID_BACK = CV_CAP_ANDROID + 99, // Android back camera - not used
		CV_CAP_ANDROID_FRONT = CV_CAP_ANDROID + 98, // Android front camera - not used

		CV_CAP_XIAPI = 1100,   // XIMEA Camera API

		CV_CAP_AVFOUNDATION = 1200,  // AVFoundation framework for iOS (OS X Lion will have the same API)

		CV_CAP_GIGANETIX = 1300,  // Smartek Giganetix GigEVisionSDK

		CV_CAP_INTELPERC = 1500, // Intel Perceptual Computing

		CV_CAP_OPENNI2 = 1600,   // OpenNI2 (for Kinect)
		CV_CAP_GPHOTO2 = 1700,
		CV_CAP_GSTREAMER = 1800, // GStreamer
		CV_CAP_FFMPEG = 1900,    // FFMPEG
		CV_CAP_IMAGES = 2000,    // OpenCV Image Sequence (e.g. img_%02d.jpg)

		CV_CAP_ARAVIS = 2100     // Aravis GigE SDK
	};

	/** @brief start capturing frames from camera: index = camera_index + domain_offset (CV_CAP_*)
	*/
	CVAPI(CvCapture*) cvCreateCameraCapture(int index);

	/** @brief grab a frame, return 1 on success, 0 on fail.

	this function is thought to be fast
	*/
	CVAPI(int) cvGrabFrame(CvCapture* capture);

	/** @brief get the frame grabbed with cvGrabFrame(..)

	This function may apply some frame processing like
	frame decompression, flipping etc.
	@warning !!!DO NOT RELEASE or MODIFY the retrieved frame!!!
	*/
	CVAPI(IplImage*) cvRetrieveFrame(CvCapture* capture, int streamIdx CV_DEFAULT(0));

	/** @brief Just a combination of cvGrabFrame and cvRetrieveFrame

	@warning !!!DO NOT RELEASE or MODIFY the retrieved frame!!!
	*/
	CVAPI(IplImage*) cvQueryFrame(CvCapture* capture);

	/** @brief stop capturing/reading and free resources
	*/
	CVAPI(void) cvReleaseCapture(CvCapture** capture);

	enum
	{
		// modes of the controlling registers (can be: auto, manual, auto single push, absolute Latter allowed with any other mode)
		// every feature can have only one mode turned on at a time
		CV_CAP_PROP_DC1394_OFF = -4,  //turn the feature off (not controlled manually nor automatically)
		CV_CAP_PROP_DC1394_MODE_MANUAL = -3, //set automatically when a value of the feature is set by the user
		CV_CAP_PROP_DC1394_MODE_AUTO = -2,
		CV_CAP_PROP_DC1394_MODE_ONE_PUSH_AUTO = -1,
		CV_CAP_PROP_POS_MSEC = 0,
		CV_CAP_PROP_POS_FRAMES = 1,
		CV_CAP_PROP_POS_AVI_RATIO = 2,
		CV_CAP_PROP_FRAME_WIDTH = 3,
		CV_CAP_PROP_FRAME_HEIGHT = 4,
		CV_CAP_PROP_FPS = 5,
		CV_CAP_PROP_FOURCC = 6,
		CV_CAP_PROP_FRAME_COUNT = 7,
		CV_CAP_PROP_FORMAT = 8,
		CV_CAP_PROP_MODE = 9,
		CV_CAP_PROP_BRIGHTNESS = 10,
		CV_CAP_PROP_CONTRAST = 11,
		CV_CAP_PROP_SATURATION = 12,
		CV_CAP_PROP_HUE = 13,
		CV_CAP_PROP_GAIN = 14,
		CV_CAP_PROP_EXPOSURE = 15,
		CV_CAP_PROP_CONVERT_RGB = 16,
		CV_CAP_PROP_WHITE_BALANCE_BLUE_U = 17,
		CV_CAP_PROP_RECTIFICATION = 18,
		CV_CAP_PROP_MONOCHROME = 19,
		CV_CAP_PROP_SHARPNESS = 20,
		CV_CAP_PROP_AUTO_EXPOSURE = 21, // exposure control done by camera,
		// user can adjust reference level
		// using this feature
		CV_CAP_PROP_GAMMA = 22,
		CV_CAP_PROP_TEMPERATURE = 23,
		CV_CAP_PROP_TRIGGER = 24,
		CV_CAP_PROP_TRIGGER_DELAY = 25,
		CV_CAP_PROP_WHITE_BALANCE_RED_V = 26,
		CV_CAP_PROP_ZOOM = 27,
		CV_CAP_PROP_FOCUS = 28,
		CV_CAP_PROP_GUID = 29,
		CV_CAP_PROP_ISO_SPEED = 30,
		CV_CAP_PROP_MAX_DC1394 = 31,
		CV_CAP_PROP_BACKLIGHT = 32,
		CV_CAP_PROP_PAN = 33,
		CV_CAP_PROP_TILT = 34,
		CV_CAP_PROP_ROLL = 35,
		CV_CAP_PROP_IRIS = 36,
		CV_CAP_PROP_SETTINGS = 37,
		CV_CAP_PROP_BUFFERSIZE = 38,
		CV_CAP_PROP_AUTOFOCUS = 39,
		CV_CAP_PROP_SAR_NUM = 40,
		CV_CAP_PROP_SAR_DEN = 41,

		CV_CAP_PROP_AUTOGRAB = 1024, // property for videoio class CvCapture_Android only
		CV_CAP_PROP_SUPPORTED_PREVIEW_SIZES_STRING = 1025, // readonly, tricky property, returns cpnst char* indeed
		CV_CAP_PROP_PREVIEW_FORMAT = 1026, // readonly, tricky property, returns cpnst char* indeed																																																																  CV_CAP_INTELPERC_GENERATORS_MASK = CV_CAP_INTELPERC_DEPTH_GENERATOR + CV_CAP_INTELPERC_IMAGE_GENERATOR
	};

	// Generic camera output modes.
	// Currently, these are supported through the libv4l interface only.
	enum
	{
		CV_CAP_MODE_BGR = 0, // BGR24 (default)
		CV_CAP_MODE_RGB = 1, // RGB24
		CV_CAP_MODE_GRAY = 2, // Y8
		CV_CAP_MODE_YUYV = 3  // YUYV
	};

	enum
	{
		// Data given from depth generator.
		CV_CAP_OPENNI_DEPTH_MAP = 0, // Depth values in mm (CV_16UC1)
		CV_CAP_OPENNI_POINT_CLOUD_MAP = 1, // XYZ in meters (CV_32FC3)
		CV_CAP_OPENNI_DISPARITY_MAP = 2, // Disparity in pixels (CV_8UC1)
		CV_CAP_OPENNI_DISPARITY_MAP_32F = 3, // Disparity in pixels (CV_32FC1)
		CV_CAP_OPENNI_VALID_DEPTH_MASK = 4, // CV_8UC1

		// Data given from RGB image generator.
		CV_CAP_OPENNI_BGR_IMAGE = 5,
		CV_CAP_OPENNI_GRAY_IMAGE = 6,

		// Data given from IR image generator.
		CV_CAP_OPENNI_IR_IMAGE = 7
	};

	// Supported output modes of OpenNI image generator
	enum
	{
		CV_CAP_OPENNI_VGA_30HZ = 0,
		CV_CAP_OPENNI_SXGA_15HZ = 1,
		CV_CAP_OPENNI_SXGA_30HZ = 2,
		CV_CAP_OPENNI_QVGA_30HZ = 3,
		CV_CAP_OPENNI_QVGA_60HZ = 4
	};

	enum
	{
		CV_CAP_INTELPERC_DEPTH_MAP = 0, // Each pixel is a 16-bit integer. The value indicates the distance from an object to the camera's XY plane or the Cartesian depth.
		CV_CAP_INTELPERC_UVDEPTH_MAP = 1, // Each pixel contains two 32-bit floating point values in the range of 0-1, representing the mapping of depth coordinates to the color coordinates.
		CV_CAP_INTELPERC_IR_MAP = 2, // Each pixel is a 16-bit integer. The value indicates the intensity of the reflected laser beam.
		CV_CAP_INTELPERC_IMAGE = 3
	};

	// gPhoto2 properties, if propertyId is less than 0 then work on widget with that __additive inversed__ camera setting ID
	// Get IDs by using CAP_PROP_GPHOTO2_WIDGET_ENUMERATE.
	// @see CvCaptureCAM_GPHOTO2 for more info
	enum
	{
		CV_CAP_PROP_GPHOTO2_PREVIEW = 17001, // Capture only preview from liveview mode.
		CV_CAP_PROP_GPHOTO2_WIDGET_ENUMERATE = 17002, // Readonly, returns (const char *).
		CV_CAP_PROP_GPHOTO2_RELOAD_CONFIG = 17003, // Trigger, only by set. Reload camera settings.
		CV_CAP_PROP_GPHOTO2_RELOAD_ON_CHANGE = 17004, // Reload all settings on set.
		CV_CAP_PROP_GPHOTO2_COLLECT_MSGS = 17005, // Collect messages with details.
		CV_CAP_PROP_GPHOTO2_FLUSH_MSGS = 17006, // Readonly, returns (const char *).
		CV_CAP_PROP_SPEED = 17007, // Exposure speed. Can be readonly, depends on camera program.
		CV_CAP_PROP_APERTURE = 17008, // Aperture. Can be readonly, depends on camera program.
		CV_CAP_PROP_EXPOSUREPROGRAM = 17009, // Camera exposure program.
		CV_CAP_PROP_VIEWFINDER = 17010  // Enter liveview mode.
	};

	/** @brief retrieve capture properties
	*/
	CVAPI(double) cvGetCaptureProperty(CvCapture* capture, int property_id);
	/** @brief set capture properties
	*/
	CVAPI(int)    cvSetCaptureProperty(CvCapture* capture, int property_id, double value);

	/** @brief Return the type of the capturer (eg, ::CV_CAP_VFW, ::CV_CAP_UNICAP)

	It is unknown if created with ::CV_CAP_ANY
	*/
	CVAPI(int)    cvGetCaptureDomain(CvCapture* capture);

	/** @brief "black box" video file writer structure

	In C++ use cv::VideoWriter
	*/
	typedef struct CvVideoWriter CvVideoWriter;

	//! Macro to construct the fourcc code of the codec. Same as CV_FOURCC()
#define CV_FOURCC_MACRO(c1, c2, c3, c4) (((c1) & 255) + (((c2) & 255) << 8) + (((c3) & 255) << 16) + (((c4) & 255) << 24))

	/** @brief Constructs the fourcc code of the codec function

	Simply call it with 4 chars fourcc code like `CV_FOURCC('I', 'Y', 'U', 'V')`

	List of codes can be obtained at [Video Codecs by FOURCC](http://www.fourcc.org/codecs.php) page.
	FFMPEG backend with MP4 container natively uses other values as fourcc code:
	see [ObjectType](http://www.mp4ra.org/codecs.html).
	*/
	CV_INLINE int CV_FOURCC(char c1, char c2, char c3, char c4)
	{
		return CV_FOURCC_MACRO(c1, c2, c3, c4);
	}

	//! (Windows only) Open Codec Selection Dialog
#define CV_FOURCC_PROMPT -1
	//! (Linux only) Use default codec for specified filename
#define CV_FOURCC_DEFAULT CV_FOURCC('I', 'Y', 'U', 'V')

	/** @brief initialize video file writer
	*/
	CVAPI(CvVideoWriter*) cvCreateVideoWriter(const char* filename, int fourcc,
		double fps, CvSize frame_size,
		int is_color CV_DEFAULT(1));

	/** @brief write frame to video file
	*/
	CVAPI(int) cvWriteFrame(CvVideoWriter* writer, const IplImage* image);

	/** @brief close video file writer
	*/
	CVAPI(void) cvReleaseVideoWriter(CvVideoWriter** writer);

	// ***************************************************************************************
	//! @name Obsolete functions/synonyms
	//! @{
#define cvCaptureFromCAM cvCreateCameraCapture //!< @deprecated use cvCreateCameraCapture() instead
#define cvCaptureFromFile cvCreateFileCapture  //!< @deprecated use cvCreateFileCapture() instead
#define cvCaptureFromAVI cvCaptureFromFile     //!< @deprecated use cvCreateFileCapture() instead
#define cvCreateAVIWriter cvCreateVideoWriter  //!< @deprecated use cvCreateVideoWriter() instead
#define cvWriteToAVI cvWriteFrame              //!< @deprecated use cvWriteFrame() instead
	//!  @} Obsolete...

	//! @} videoio_c

#ifdef __cplusplus
}
#endif

#endif //OPENCV_VIDEOIO_H

