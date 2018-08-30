#include "precomp.hpp"

#ifdef HAVE_POSIX_MEMALIGN
#include <stdlib.h>
#elif defined HAVE_MALLOC_H
#include <malloc.h>
#endif

namespace cv {

	static void* OutOfMemoryError(size_t size)
	{
//		CV_Error_(CV_StsNoMem, ("Failed to allocate %llu bytes", (unsigned long long)size));
		return 0;
	}


	void* fastMalloc(size_t size)
	{
#ifdef HAVE_POSIX_MEMALIGN
		void* ptr = NULL;
		if (posix_memalign(&ptr, CV_MALLOC_ALIGN, size))
			ptr = NULL;
		if (!ptr)
			return OutOfMemoryError(size);
		return ptr;
#elif defined HAVE_MEMALIGN
		void* ptr = memalign(CV_MALLOC_ALIGN, size);
		if (!ptr)
			return OutOfMemoryError(size);
		return ptr;
#else
		uchar* udata = (uchar*)malloc(size + sizeof(void*) + CV_MALLOC_ALIGN);
		if (!udata)
			return OutOfMemoryError(size);
		uchar** adata = alignPtr((uchar**)udata + 1, CV_MALLOC_ALIGN);
		adata[-1] = udata;
		return adata;
#endif
	}

	void fastFree(void* ptr)
	{
#if defined HAVE_POSIX_MEMALIGN || defined HAVE_MEMALIGN
		free(ptr);
#else
		if (ptr)
		{
			uchar* udata = ((uchar**)ptr)[-1];
			CV_DbgAssert(udata < (uchar*)ptr &&
				((uchar*)ptr - udata) <= (ptrdiff_t)(sizeof(void*) + CV_MALLOC_ALIGN));
			free(udata);
		}
#endif
	}

} // namespace

CV_IMPL void* cvAlloc(size_t size)
{
	return cv::fastMalloc(size);
}

CV_IMPL void cvFree_(void* ptr)
{
	cv::fastFree(ptr);
}

/* End of file. */
