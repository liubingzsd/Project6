#ifndef __OPENCV_CORE_BUFFER_POOL_IMPL_HPP__
#define __OPENCV_CORE_BUFFER_POOL_IMPL_HPP__

#include "../include/opencv2/core/bufferpool.hpp"

namespace cv {

	class DummyBufferPoolController : public BufferPoolController
	{
	public:
		DummyBufferPoolController() { }
		virtual ~DummyBufferPoolController() { }

		virtual size_t getReservedSize() const { return (size_t)-1; }
		virtual size_t getMaxReservedSize() const { return (size_t)-1; }
		virtual void setMaxReservedSize(size_t size) { (void)size; }
		virtual void freeAllReservedBuffers() { }
	};

} // namespace

#endif // __OPENCV_CORE_BUFFER_POOL_IMPL_HPP__
