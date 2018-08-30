#include "precomp.hpp"

namespace cv
{

	using std::vector;

	Feature2D::~Feature2D() {}

	/*
	* Detect keypoints in an image.
	* image        The image.
	* keypoints    The detected keypoints.
	* mask         Mask specifying where to look for keypoints (optional). Must be a char
	*              matrix with non-zero values in the region of interest.
	*/
	void Feature2D::detect(InputArray image,
		std::vector<KeyPoint>& keypoints,
		InputArray mask)
	{
		CV_INSTRUMENT_REGION()

			if (image.empty())
			{
				keypoints.clear();
				return;
			}
		detectAndCompute(image, mask, keypoints, noArray(), false);
	}


	void Feature2D::detect(InputArrayOfArrays _images,
		std::vector<std::vector<KeyPoint> >& keypoints,
		InputArrayOfArrays _masks)
	{
		CV_INSTRUMENT_REGION()

		vector<Mat> images, masks;

		_images.getMatVector(images);
		size_t i, nimages = images.size();

		if (!_masks.empty())
		{
			_masks.getMatVector(masks);
			CV_Assert(masks.size() == nimages);
		}

		keypoints.resize(nimages);

		for (i = 0; i < nimages; i++)
		{
			detect(images[i], keypoints[i], masks.empty() ? Mat() : masks[i]);
		}
	}

	/*
	* Compute the descriptors for a set of keypoints in an image.
	* image        The image.
	* keypoints    The input keypoints. Keypoints for which a descriptor cannot be computed are removed.
	* descriptors  Copmputed descriptors. Row i is the descriptor for keypoint i.
	*/
	void Feature2D::compute(InputArray image,
		std::vector<KeyPoint>& keypoints,
		OutputArray descriptors)
	{
		CV_INSTRUMENT_REGION()

			if (image.empty())
			{
				descriptors.release();
				return;
			}
		detectAndCompute(image, noArray(), keypoints, descriptors, true);
	}

	void Feature2D::compute(InputArrayOfArrays _images,
		std::vector<std::vector<KeyPoint> >& keypoints,
		OutputArrayOfArrays _descriptors)
	{
		CV_INSTRUMENT_REGION()

			if (!_descriptors.needed())
				return;

		vector<Mat> images;

		_images.getMatVector(images);
		size_t i, nimages = images.size();

		CV_Assert(keypoints.size() == nimages);
		CV_Assert(_descriptors.kind() == _InputArray::STD_VECTOR_MAT);

		vector<Mat>& descriptors = *(vector<Mat>*)_descriptors.getObj();
		descriptors.resize(nimages);

		for (i = 0; i < nimages; i++)
		{
			compute(images[i], keypoints[i], descriptors[i]);
		}
	}


	/* Detects keypoints and computes the descriptors */
	void Feature2D::detectAndCompute(InputArray, InputArray,
		std::vector<KeyPoint>&,
		OutputArray,
		bool)
	{
		CV_INSTRUMENT_REGION()

			CV_Error(Error::StsNotImplemented, "");
	}

	void Feature2D::write(const String& fileName) const
	{
		FileStorage fs(fileName, FileStorage::WRITE);
		write(fs);
	}

	void Feature2D::read(const String& fileName)
	{
		FileStorage fs(fileName, FileStorage::READ);
		read(fs.root());
	}

	void Feature2D::write(FileStorage&) const
	{
	}

	void Feature2D::read(const FileNode&)
	{
	}

	int Feature2D::descriptorSize() const
	{
		return 0;
	}

	int Feature2D::descriptorType() const
	{
		return CV_32F;
	}

	int Feature2D::defaultNorm() const
	{
		int tp = descriptorType();
		return tp == CV_8U ? NORM_HAMMING : NORM_L2;
	}

	// Return true if detector object is empty
	bool Feature2D::empty() const
	{
		return true;
	}

	String Feature2D::getDefaultName() const
	{
		return "Feature2D";
	}

}
