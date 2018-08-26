#ifndef __OPENCV_IMGPROC_FILTERENGINE_HPP__
#define __OPENCV_IMGPROC_FILTERENGINE_HPP__

#include "../include/opencv2/imgproc.hpp"

namespace cv
{
	enum
	{
		KERNEL_GENERAL = 0, // the kernel is generic. No any type of symmetry or other properties.
		KERNEL_SYMMETRICAL = 1, // kernel[i] == kernel[ksize-i-1] , and the anchor is at the center
		KERNEL_ASYMMETRICAL = 2, // kernel[i] == -kernel[ksize-i-1] , and the anchor is at the center
		KERNEL_SMOOTH = 4, // all the kernel elements are non-negative and summed to 1
		KERNEL_INTEGER = 8  // all the kernel coefficients are integer numbers
	};

	class BaseRowFilter
	{
	public:
		//! the default constructor
		BaseRowFilter();
		//! the destructor
		virtual ~BaseRowFilter();
		//! the filtering operator. Must be overridden in the derived classes. The horizontal border interpolation is done outside of the class.
		virtual void operator()(const uchar* src, uchar* dst, int width, int cn) = 0;

		int ksize;
		int anchor;
	};

	class BaseColumnFilter
	{
	public:
		//! the default constructor
		BaseColumnFilter();
		//! the destructor
		virtual ~BaseColumnFilter();
		//! the filtering operator. Must be overridden in the derived classes. The vertical border interpolation is done outside of the class.
		virtual void operator()(const uchar** src, uchar* dst, int dststep, int dstcount, int width) = 0;
		//! resets the internal buffers, if any
		virtual void reset();

		int ksize;
		int anchor;
	};

	class BaseFilter
	{
	public:
		//! the default constructor
		BaseFilter();
		//! the destructor
		virtual ~BaseFilter();
		//! the filtering operator. The horizontal and the vertical border interpolation is done outside of the class.
		virtual void operator()(const uchar** src, uchar* dst, int dststep, int dstcount, int width, int cn) = 0;
		//! resets the internal buffers, if any
		virtual void reset();

		Size ksize;
		Point anchor;
	};

	class FilterEngine
	{
	public:
		//! the default constructor
		FilterEngine();
		//! the full constructor. Either _filter2D or both _rowFilter and _columnFilter must be non-empty.
		FilterEngine(const Ptr<BaseFilter>& _filter2D,
			const Ptr<BaseRowFilter>& _rowFilter,
			const Ptr<BaseColumnFilter>& _columnFilter,
			int srcType, int dstType, int bufType,
			int _rowBorderType = BORDER_REPLICATE,
			int _columnBorderType = -1,
			const Scalar& _borderValue = Scalar());
		//! the destructor
		virtual ~FilterEngine();
		//! reinitializes the engine. The previously assigned filters are released.
		void init(const Ptr<BaseFilter>& _filter2D,
			const Ptr<BaseRowFilter>& _rowFilter,
			const Ptr<BaseColumnFilter>& _columnFilter,
			int srcType, int dstType, int bufType,
			int _rowBorderType = BORDER_REPLICATE,
			int _columnBorderType = -1,
			const Scalar& _borderValue = Scalar());

		//! starts filtering of the specified ROI of an image of size wholeSize.
		virtual int start(const cv::Size &wholeSize, const cv::Size &sz, const cv::Point &ofs);
		//! starts filtering of the specified ROI of the specified image.
		virtual int start(const Mat& src, const cv::Size &wsz, const cv::Point &ofs);
		//! processes the next srcCount rows of the image.
		virtual int proceed(const uchar* src, int srcStep, int srcCount,
			uchar* dst, int dstStep);
		//! applies filter to the specified ROI of the image. if srcRoi=(0,0,-1,-1), the whole image is filtered.
		virtual void apply(const Mat& src, Mat& dst, const cv::Size &wsz, const cv::Point &ofs);

		//! returns true if the filter is separable
		bool isSeparable() const { return !filter2D; }
		//! returns the number
		int remainingInputRows() const;
		int remainingOutputRows() const;

		int srcType;
		int dstType;
		int bufType;
		Size ksize;
		Point anchor;
		int maxWidth;
		Size wholeSize;
		Rect roi;
		int dx1;
		int dx2;
		int rowBorderType;
		int columnBorderType;
		std::vector<int> borderTab;
		int borderElemSize;
		std::vector<uchar> ringBuf;
		std::vector<uchar> srcRow;
		std::vector<uchar> constBorderValue;
		std::vector<uchar> constBorderRow;
		int bufStep;
		int startY;
		int startY0;
		int endY;
		int rowCount;
		int dstY;
		std::vector<uchar*> rows;

		Ptr<BaseFilter> filter2D;
		Ptr<BaseRowFilter> rowFilter;
		Ptr<BaseColumnFilter> columnFilter;
	};
	//! returns type (one of KERNEL_*) of 1D or 2D kernel specified by its coefficients.
	int getKernelType(InputArray kernel, Point anchor);

	//! returns the primitive row filter with the specified kernel
	Ptr<BaseRowFilter> getLinearRowFilter(int srcType, int bufType,
		InputArray kernel, int anchor,
		int symmetryType);

	//! returns the primitive column filter with the specified kernel
	Ptr<BaseColumnFilter> getLinearColumnFilter(int bufType, int dstType,
		InputArray kernel, int anchor,
		int symmetryType, double delta = 0,
		int bits = 0);

	//! returns 2D filter with the specified kernel
	Ptr<BaseFilter> getLinearFilter(int srcType, int dstType,
		InputArray kernel,
		Point anchor = Point(-1, -1),
		double delta = 0, int bits = 0);

	//! returns the separable linear filter engine
	Ptr<FilterEngine> createSeparableLinearFilter(int srcType, int dstType,
		InputArray rowKernel, InputArray columnKernel,
		Point anchor = Point(-1, -1), double delta = 0,
		int rowBorderType = BORDER_DEFAULT,
		int columnBorderType = -1,
		const Scalar& borderValue = Scalar());

	//! returns the non-separable linear filter engine
	Ptr<FilterEngine> createLinearFilter(int srcType, int dstType,
		InputArray kernel, Point _anchor = Point(-1, -1),
		double delta = 0, int rowBorderType = BORDER_DEFAULT,
		int columnBorderType = -1, const Scalar& borderValue = Scalar());

	//! returns the Gaussian filter engine
	Ptr<FilterEngine> createGaussianFilter(int type, Size ksize,
		double sigma1, double sigma2 = 0,
		int borderType = BORDER_DEFAULT);

	//! returns filter engine for the generalized Sobel operator
	Ptr<FilterEngine> createDerivFilter(int srcType, int dstType,
		int dx, int dy, int ksize,
		int borderType = BORDER_DEFAULT);

	//! returns horizontal 1D box filter
	Ptr<BaseRowFilter> getRowSumFilter(int srcType, int sumType,
		int ksize, int anchor = -1);

	//! returns vertical 1D box filter
	Ptr<BaseColumnFilter> getColumnSumFilter(int sumType, int dstType,
		int ksize, int anchor = -1,
		double scale = 1);
	//! returns box filter engine
	Ptr<FilterEngine> createBoxFilter(int srcType, int dstType, Size ksize,
		Point anchor = Point(-1, -1),
		bool normalize = true,
		int borderType = BORDER_DEFAULT);


	//! returns horizontal 1D morphological filter
	Ptr<BaseRowFilter> getMorphologyRowFilter(int op, int type, int ksize, int anchor = -1);

	//! returns vertical 1D morphological filter
	Ptr<BaseColumnFilter> getMorphologyColumnFilter(int op, int type, int ksize, int anchor = -1);

	//! returns 2D morphological filter
	Ptr<BaseFilter> getMorphologyFilter(int op, int type, InputArray kernel,
		Point anchor = Point(-1, -1));

	//! returns morphological filter engine. Only MORPH_ERODE and MORPH_DILATE are supported.
	CV_EXPORTS Ptr<FilterEngine> createMorphologyFilter(int op, int type, InputArray kernel,
		Point anchor = Point(-1, -1), int rowBorderType = BORDER_CONSTANT,
		int columnBorderType = -1,
		const Scalar& borderValue = morphologyDefaultBorderValue());

	static inline Point normalizeAnchor(Point anchor, Size ksize)
	{
		if (anchor.x == -1)
			anchor.x = ksize.width / 2;
		if (anchor.y == -1)
			anchor.y = ksize.height / 2;
		CV_Assert(anchor.inside(Rect(0, 0, ksize.width, ksize.height)));
		return anchor;
	}

	void preprocess2DKernel(const Mat& kernel, std::vector<Point>& coords, std::vector<uchar>& coeffs);
	void crossCorr(const Mat& src, const Mat& templ, Mat& dst,
		Size corrsize, int ctype,
		Point anchor = Point(0, 0), double delta = 0,
		int borderType = BORDER_REFLECT_101);



}

#endif
