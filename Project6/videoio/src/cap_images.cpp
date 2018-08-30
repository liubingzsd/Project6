#include "precomp.hpp"
#include <sys/stat.h>

#ifdef NDEBUG
#define CV_WARN(message)
#else
#define CV_WARN(message) fprintf(stderr, "warning: %s (%s:%d)\n", message, __FILE__, __LINE__)
#endif

#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif

class CvCapture_Images : public CvCapture
{
public:
	CvCapture_Images()
	{
		filename = NULL;
		currentframe = firstframe = 0;
		length = 0;
		frame = NULL;
		grabbedInOpen = false;
	}

	virtual ~CvCapture_Images()
	{
		close();
	}

	virtual bool open(const char* _filename);
	virtual void close();
	virtual double getProperty(int) const;
	virtual bool setProperty(int, double);
	virtual bool grabFrame();
	virtual IplImage* retrieveFrame(int);

protected:
	char*  filename; // actually a printf-pattern
	unsigned currentframe;
	unsigned firstframe; // number of first frame
	unsigned length; // length of sequence

	IplImage* frame;
	bool grabbedInOpen;
};


void CvCapture_Images::close()
{
	if (filename)
	{
		free(filename);
		filename = NULL;
	}
	currentframe = firstframe = 0;
	length = 0;
	cvReleaseImage(&frame);
}


bool CvCapture_Images::grabFrame()
{
	char str[_MAX_PATH];
	sprintf(str, filename, firstframe + currentframe);

	if (grabbedInOpen)
	{
		grabbedInOpen = false;
		++currentframe;

		return frame != NULL;
	}

	cvReleaseImage(&frame);
	frame;// = cvLoadImage(str, CV_LOAD_IMAGE_UNCHANGED);
	if (frame)
		currentframe++;

	return frame != NULL;
}

IplImage* CvCapture_Images::retrieveFrame(int)
{
	return grabbedInOpen ? NULL : frame;
}

double CvCapture_Images::getProperty(int id) const
{
	switch (id)
	{
	case CV_CAP_PROP_POS_MSEC:
		CV_WARN("collections of images don't have framerates\n");
		return 0;
	case CV_CAP_PROP_POS_FRAMES:
		return currentframe;
	case CV_CAP_PROP_FRAME_COUNT:
		return length;
	case CV_CAP_PROP_POS_AVI_RATIO:
		return (double)currentframe / (double)(length - 1);
	case CV_CAP_PROP_FRAME_WIDTH:
		return frame ? frame->width : 0;
	case CV_CAP_PROP_FRAME_HEIGHT:
		return frame ? frame->height : 0;
	case CV_CAP_PROP_FPS:
		CV_WARN("collections of images don't have framerates\n");
		return 1;
	case CV_CAP_PROP_FOURCC:
		CV_WARN("collections of images don't have 4-character codes\n");
		return 0;
	}
	return 0;
}

bool CvCapture_Images::setProperty(int id, double value)
{
	switch (id)
	{
	case CV_CAP_PROP_POS_MSEC:
	case CV_CAP_PROP_POS_FRAMES:
		if (value < 0) {
			CV_WARN("seeking to negative positions does not work - clamping\n");
			value = 0;
		}
		if (value >= length) {
			CV_WARN("seeking beyond end of sequence - clamping\n");
			value = length - 1;
		}
		currentframe = cvRound(value);
		if (currentframe != 0)
			grabbedInOpen = false; // grabbed frame is not valid anymore
		return true;
	case CV_CAP_PROP_POS_AVI_RATIO:
		if (value > 1) {
			CV_WARN("seeking beyond end of sequence - clamping\n");
			value = 1;
		}
		else if (value < 0) {
			CV_WARN("seeking to negative positions does not work - clamping\n");
			value = 0;
		}
		currentframe = cvRound((length - 1) * value);
		if (currentframe != 0)
			grabbedInOpen = false; // grabbed frame is not valid anymore
		return true;
	}
	CV_WARN("unknown/unhandled property\n");
	return false;
}

static char* icvExtractPattern(const char *filename, unsigned *offset)
{
	char *name = (char *)filename;

	if (!filename)
		return 0;

	// check whether this is a valid image sequence filename
	char *at = strchr(name, '%');
	if (at)
	{
		unsigned int dummy;
		if (sscanf(at + 1, "%ud", &dummy) != 1)
			return 0;
		name = _strdup(filename);
	}
	else // no pattern filename was given - extract the pattern
	{
		at = name;

		// ignore directory names
		char *slash = strrchr(at, '/');
		if (slash) at = slash + 1;

#ifdef _WIN32
		slash = strrchr(at, '\\');
		if (slash) at = slash + 1;
#endif

		while (*at && !isdigit(*at)) at++;

		if (!*at)
			return 0;

		sscanf(at, "%u", offset);

		int size = (int)strlen(filename) + 20;
		name = (char *)malloc(size);
		CV_Assert(name != NULL);
		strncpy(name, filename, at - filename);
		name[at - filename] = 0;

		strcat(name, "%0");

		int i;
		char *extension;
		for (i = 0, extension = at; isdigit(at[i]); i++, extension++)
			;
		char places[13] = { 0 };
		sprintf(places, "%dd", i);

		strcat(name, places);
		strcat(name, extension);
	}

	return name;
}


bool CvCapture_Images::open(const char * _filename)
{
	unsigned offset = 0;
	close();

	filename = icvExtractPattern(_filename, &offset);
	if (!filename)
		return false;

	// determine the length of the sequence
	length = 0;
	char str[_MAX_PATH];
	for (;;)
	{
		sprintf(str, filename, offset + length);
		struct stat s;
		if (stat(str, &s))
		{
			if (length == 0 && offset == 0) // allow starting with 0 or 1
			{
				offset++;
				continue;
			}
		}

//		if (!cvHaveImageReader(str))
//			break;

		length++;
	}

	if (length == 0)
	{
		close();
		return false;
	}

	firstframe = offset;

	// grab frame to enable properties retrieval
	bool grabRes = grabFrame();
	grabbedInOpen = true;
	currentframe = 0;

	return grabRes;
}


CvCapture* cvCreateFileCapture_Images(const char * filename)
{
	CvCapture_Images* capture = new CvCapture_Images;

	if (capture->open(filename))
		return capture;

	delete capture;
	return NULL;
}

//
//
// image sequence writer
//
//
class CvVideoWriter_Images : public CvVideoWriter
{
public:
	CvVideoWriter_Images()
	{
		filename = 0;
		currentframe = 0;
	}
	virtual ~CvVideoWriter_Images() { close(); }

	virtual bool open(const char* _filename);
	virtual void close();
	virtual bool setProperty(int, double);
	virtual bool writeFrame(const IplImage*);

protected:
	char* filename;
	unsigned currentframe;
	std::vector<int> params;
};


bool CvVideoWriter_Images::writeFrame(const IplImage* image)
{
	char str[_MAX_PATH];
	sprintf(str, filename, currentframe);
	std::vector<int> image_params = params;
	image_params.push_back(0); // append parameters 'stop' mark
	image_params.push_back(0);
	int ret = 0;// = cvSaveImage(str, image, &image_params[0]);

	currentframe++;

	return ret > 0;
}

void CvVideoWriter_Images::close()
{
	if (filename)
	{
		free(filename);
		filename = 0;
	}
	currentframe = 0;
	params.clear();
}


bool CvVideoWriter_Images::open(const char* _filename)
{
	unsigned offset = 0;

	close();

	filename = icvExtractPattern(_filename, &offset);
	if (!filename)
		return false;

	char str[_MAX_PATH];
	sprintf(str, filename, 0);
//	if (!cvHaveImageWriter(str))
	{
		close();
		return false;
	}

	currentframe = offset;
	params.clear();
	return true;
}


bool CvVideoWriter_Images::setProperty(int id, double value)
{
	if (id >= cv::CAP_PROP_IMAGES_BASE && id < cv::CAP_PROP_IMAGES_LAST)
	{
		params.push_back(id - cv::CAP_PROP_IMAGES_BASE);
		params.push_back(static_cast<int>(value));
		return true;
	}
	return false; // not supported
}


CvVideoWriter* cvCreateVideoWriter_Images(const char* filename)
{
	CvVideoWriter_Images *writer = new CvVideoWriter_Images;

	if (writer->open(filename))
		return writer;

	delete writer;
	return 0;
}
