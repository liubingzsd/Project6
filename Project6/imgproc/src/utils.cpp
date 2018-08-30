#include "precomp.hpp"

CV_IMPL CvSeq* cvPointSeqFromMat(int seq_kind, const CvArr* arr,
	CvContour* contour_header, CvSeqBlock* block)
{
	CV_Assert(arr != 0 && contour_header != 0 && block != 0);

	int eltype;
	CvMat hdr;
	CvMat* mat = (CvMat*)arr;

	if (!CV_IS_MAT(mat))
		CV_Error(CV_StsBadArg, "Input array is not a valid matrix");

	if (CV_MAT_CN(mat->type) == 1 && mat->width == 2)
		mat = cvReshape(mat, &hdr, 2);

	eltype = CV_MAT_TYPE(mat->type);
	if (eltype != CV_32SC2 && eltype != CV_32FC2)
		CV_Error(CV_StsUnsupportedFormat,
			"The matrix can not be converted to point sequence because of "
			"inappropriate element type");

	if ((mat->width != 1 && mat->height != 1) || !CV_IS_MAT_CONT(mat->type))
		CV_Error(CV_StsBadArg,
			"The matrix converted to point sequence must be "
			"1-dimensional and continuous");

	cvMakeSeqHeaderForArray(
		(seq_kind & (CV_SEQ_KIND_MASK | CV_SEQ_FLAG_CLOSED)) | eltype,
		sizeof(CvContour), CV_ELEM_SIZE(eltype), mat->data.ptr,
		mat->width*mat->height, (CvSeq*)contour_header, block);

	return (CvSeq*)contour_header;
}

CV_IMPL void
cvCopyMakeBorder(const CvArr* srcarr, CvArr* dstarr, CvPoint offset,
	int borderType, CvScalar value)
{
	cv::Mat src = cv::cvarrToMat(srcarr), dst = cv::cvarrToMat(dstarr);
	int left = offset.x, right = dst.cols - src.cols - left;
	int top = offset.y, bottom = dst.rows - src.rows - top;

	CV_Assert(dst.type() == src.type());
	cv::copyMakeBorder(src, dst, top, bottom, left, right, borderType, value);
}

/* End of file. */
