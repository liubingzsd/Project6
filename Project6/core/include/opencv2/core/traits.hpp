#ifndef OPENCV_CORE_TRAITS_HPP
#define OPENCV_CORE_TRAITS_HPP

#include "cvdef.h"

namespace cv
{

	template<typename _Tp> class DataType
	{
	public:
#ifdef OPENCV_TRAITS_ENABLE_DEPRECATED
		typedef _Tp         value_type;
		typedef value_type  work_type;
		typedef value_type  channel_type;
		typedef value_type  vec_type;
		enum {
			generic_type = 1,
			depth = -1,
			channels = 1,
			fmt = 0,
			type = CV_MAKETYPE(depth, channels)
		};
#endif
	};

	template<> class DataType<bool>
	{
	public:
		typedef bool        value_type;
		typedef int         work_type;
		typedef value_type  channel_type;
		typedef value_type  vec_type;
		enum {
			generic_type = 0,
			depth = CV_8U,
			channels = 1,
			fmt = (int)'u',
			type = CV_MAKETYPE(depth, channels)
		};
	};

	template<> class DataType<uchar>
	{
	public:
		typedef uchar       value_type;
		typedef int         work_type;
		typedef value_type  channel_type;
		typedef value_type  vec_type;
		enum {
			generic_type = 0,
			depth = CV_8U,
			channels = 1,
			fmt = (int)'u',
			type = CV_MAKETYPE(depth, channels)
		};
	};

	template<> class DataType<schar>
	{
	public:
		typedef schar       value_type;
		typedef int         work_type;
		typedef value_type  channel_type;
		typedef value_type  vec_type;
		enum {
			generic_type = 0,
			depth = CV_8S,
			channels = 1,
			fmt = (int)'c',
			type = CV_MAKETYPE(depth, channels)
		};
	};

	template<> class DataType<char>
	{
	public:
		typedef schar       value_type;
		typedef int         work_type;
		typedef value_type  channel_type;
		typedef value_type  vec_type;
		enum {
			generic_type = 0,
			depth = CV_8S,
			channels = 1,
			fmt = (int)'c',
			type = CV_MAKETYPE(depth, channels)
		};
	};

	template<> class DataType<ushort>
	{
	public:
		typedef ushort      value_type;
		typedef int         work_type;
		typedef value_type  channel_type;
		typedef value_type  vec_type;
		enum {
			generic_type = 0,
			depth = CV_16U,
			channels = 1,
			fmt = (int)'w',
			type = CV_MAKETYPE(depth, channels)
		};
	};

	template<> class DataType<short>
	{
	public:
		typedef short       value_type;
		typedef int         work_type;
		typedef value_type  channel_type;
		typedef value_type  vec_type;
		enum {
			generic_type = 0,
			depth = CV_16S,
			channels = 1,
			fmt = (int)'s',
			type = CV_MAKETYPE(depth, channels)
		};
	};

	template<> class DataType<int>
	{
	public:
		typedef int         value_type;
		typedef value_type  work_type;
		typedef value_type  channel_type;
		typedef value_type  vec_type;
		enum {
			generic_type = 0,
			depth = CV_32S,
			channels = 1,
			fmt = (int)'i',
			type = CV_MAKETYPE(depth, channels)
		};
	};

	template<> class DataType<float>
	{
	public:
		typedef float       value_type;
		typedef value_type  work_type;
		typedef value_type  channel_type;
		typedef value_type  vec_type;
		enum {
			generic_type = 0,
			depth = CV_32F,
			channels = 1,
			fmt = (int)'f',
			type = CV_MAKETYPE(depth, channels)
		};
	};

	template<> class DataType<double>
	{
	public:
		typedef double      value_type;
		typedef value_type  work_type;
		typedef value_type  channel_type;
		typedef value_type  vec_type;
		enum {
			generic_type = 0,
			depth = CV_64F,
			channels = 1,
			fmt = (int)'d',
			type = CV_MAKETYPE(depth, channels)
		};
	};


	/** @brief A helper class for cv::DataType

	The class is specialized for each fundamental numerical data type supported by OpenCV. It provides
	DataDepth<T>::value constant.
	*/
	template<typename _Tp> class DataDepth
	{
	public:
		enum
		{
			value = DataType<_Tp>::depth,
			fmt = DataType<_Tp>::fmt
		};
	};


#ifdef OPENCV_TRAITS_ENABLE_DEPRECATED

	template<int _depth> class TypeDepth
	{
#ifdef OPENCV_TRAITS_ENABLE_LEGACY_DEFAULTS
		enum { depth = CV_USRTYPE1 };
		typedef void value_type;
#endif
	};

	template<> class TypeDepth<CV_8U>
	{
		enum { depth = CV_8U };
		typedef uchar value_type;
	};

	template<> class TypeDepth<CV_8S>
	{
		enum { depth = CV_8S };
		typedef schar value_type;
	};

	template<> class TypeDepth<CV_16U>
	{
		enum { depth = CV_16U };
		typedef ushort value_type;
	};

	template<> class TypeDepth<CV_16S>
	{
		enum { depth = CV_16S };
		typedef short value_type;
	};

	template<> class TypeDepth<CV_32S>
	{
		enum { depth = CV_32S };
		typedef int value_type;
	};

	template<> class TypeDepth<CV_32F>
	{
		enum { depth = CV_32F };
		typedef float value_type;
	};

	template<> class TypeDepth<CV_64F>
	{
		enum { depth = CV_64F };
		typedef double value_type;
	};

#endif

	//! @}

	namespace traits {

		namespace internal {
#define CV_CREATE_MEMBER_CHECK(X) \
template<typename T> class CheckMember_##X { \
    struct Fallback { int X; }; \
    struct Derived : T, Fallback { }; \
    template<typename U, U> struct Check; \
    typedef char CV_NO[1]; \
    typedef char CV_YES[2]; \
    template<typename U> static CV_NO & func(Check<int Fallback::*, &U::X> *); \
    template<typename U> static CV_YES & func(...); \
public: \
    typedef CheckMember_##X type; \
    enum { value = sizeof(func<Derived>(0)) == sizeof(CV_YES) }; \
};

			CV_CREATE_MEMBER_CHECK(fmt)
				CV_CREATE_MEMBER_CHECK(type)

		} // namespace internal


		template<typename T>
		struct Depth
		{
			enum { value = DataType<T>::depth };
		};

		template<typename T>
		struct Type
		{
			enum { value = DataType<T>::type };
		};

		/** Similar to traits::Type<T> but has value = -1 in case of unknown type (instead of compiler error) */
		template<typename T, bool available = internal::CheckMember_type< DataType<T> >::value >
		struct SafeType {};

		template<typename T>
		struct SafeType<T, false>
		{
			enum { value = -1 };
		};

		template<typename T>
		struct SafeType<T, true>
		{
			enum { value = Type<T>::value };
		};


		template<typename T, bool available = internal::CheckMember_fmt< DataType<T> >::value >
		struct SafeFmt {};

		template<typename T>
		struct SafeFmt<T, false>
		{
			enum { fmt = 0 };
		};

		template<typename T>
		struct SafeFmt<T, true>
		{
			enum { fmt = DataType<T>::fmt };
		};


	} // namespace

} // cv

#endif // OPENCV_CORE_TRAITS_HPP

