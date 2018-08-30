#include "precomp.hpp"

namespace cv
{

	Algorithm::Algorithm()
	{
		CV_TRACE_FUNCTION();
	}

	Algorithm::~Algorithm()
	{
		CV_TRACE_FUNCTION();
	}

	void Algorithm::write(const Ptr<FileStorage>& fs, const String& name) const
	{
		CV_TRACE_FUNCTION();
		if (name.empty())
		{
			write(*fs);
			return;
		}
		*fs << name << "{";
		write(*fs);
		*fs << "}";
	}

	void Algorithm::save(const String& filename) const
	{
		CV_TRACE_FUNCTION();
		FileStorage fs(filename, FileStorage::WRITE);
		fs << getDefaultName() << "{";
		write(fs);
		fs << "}";
	}

	String Algorithm::getDefaultName() const
	{
		CV_TRACE_FUNCTION();
		return String("my_object");
	}

	void Algorithm::writeFormat(FileStorage& fs) const
	{
		CV_TRACE_FUNCTION();
		fs << "format" << (int)3;
	}

}

/* End of file. */
