#ifndef OPENCV_CONFIGURATION_PRIVATE_HPP
#define OPENCV_CONFIGURATION_PRIVATE_HPP

namespace cv {
	namespace utils {

		CV_EXPORTS bool getConfigurationParameterBool(const char* name, bool defaultValue);
		CV_EXPORTS size_t getConfigurationParameterSizeT(const char* name, size_t defaultValue);
		CV_EXPORTS cv::String getConfigurationParameterString(const char* name, const char* defaultValue);

	}
} // namespace

#endif // OPENCV_CONFIGURATION_PRIVATE_HPP

