#ifndef OPENCV_CORE_PARALLEL_IMPL_HPP
#define OPENCV_CORE_PARALLEL_IMPL_HPP

namespace cv {

	unsigned defaultNumberOfThreads();
	void parallel_for_pthreads(const Range& range, const ParallelLoopBody& body, double nstripes);
	size_t parallel_pthreads_get_threads_num();
	void parallel_pthreads_set_threads_num(int num);

}

#endif // OPENCV_CORE_PARALLEL_IMPL_HPP

