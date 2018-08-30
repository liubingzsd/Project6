#ifndef OPENCV_CORE_SRC_UMATRIX_HPP
#define OPENCV_CORE_SRC_UMATRIX_HPP

namespace cv {

	struct CV_EXPORTS UMatDataAutoLock
	{
		explicit UMatDataAutoLock(UMatData* u);
		UMatDataAutoLock(UMatData* u1, UMatData* u2);
		~UMatDataAutoLock();
		UMatData* u1;
		UMatData* u2;
	};

}

#endif // OPENCV_CORE_SRC_UMATRIX_HPP
