#include "precomp.hpp"

char* cv::String::allocate(size_t len)
{
	size_t totalsize = alignSize(len + 1, (int)sizeof(int));
	int* data = (int*)cv::fastMalloc(totalsize + sizeof(int));
	data[0] = 1;
	cstr_ = (char*)(data + 1);
	len_ = len;
	cstr_[len] = 0;
	return cstr_;
}


void cv::String::deallocate()
{
	int* data = (int*)cstr_;
	len_ = 0;
	cstr_ = 0;

	if (data && 1 == CV_XADD(data - 1, -1))
	{
		cv::fastFree(data - 1);
	}
}
