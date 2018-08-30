#ifndef OPENCV_CORE_PERSISTENCE_HPP
#define OPENCV_CORE_PERSISTENCE_HPP

#ifndef CV_DOXYGEN
/// Define to support persistence legacy formats
#define CV__LEGACY_PERSISTENCE
#endif

#ifndef __cplusplus
#  error persistence.hpp header must be compiled as C++
#endif


typedef struct CvFileStorage CvFileStorage;
typedef struct CvFileNode CvFileNode;
typedef struct CvMat CvMat;
typedef struct CvMatND CvMatND;

//! @} core_c

#include "types.hpp"
#include "mat.hpp"

namespace cv {

	////////////////////////// XML & YAML I/O //////////////////////////

	class CV_EXPORTS FileNode;
	class CV_EXPORTS FileNodeIterator;

	/** @brief XML/YAML/JSON file storage class that encapsulates all the information necessary for writing or
	reading data to/from a file.
	*/
	class CV_EXPORTS_W FileStorage
	{
	public:
		//! file storage mode
		enum Mode
		{
			READ = 0, //!< value, open the file for reading
			WRITE = 1, //!< value, open the file for writing
			APPEND = 2, //!< value, open the file for appending
			MEMORY = 4, //!< flag, read data from source or write data to the internal buffer (which is
			//!< returned by FileStorage::release)
			FORMAT_MASK = (7 << 3), //!< mask for format flags
			FORMAT_AUTO = 0,      //!< flag, auto format
			FORMAT_XML = (1 << 3), //!< flag, XML format
			FORMAT_YAML = (2 << 3), //!< flag, YAML format
			FORMAT_JSON = (3 << 3), //!< flag, JSON format

			BASE64 = 64,     //!< flag, write rawdata in Base64 by default. (consider using WRITE_BASE64)
			WRITE_BASE64 = BASE64 | WRITE, //!< flag, enable both WRITE and BASE64
		};
		enum
		{
			UNDEFINED = 0,
			VALUE_EXPECTED = 1,
			NAME_EXPECTED = 2,
			INSIDE_MAP = 4
		};

		/** @brief The constructors.

		The full constructor opens the file. Alternatively you can use the default constructor and then
		call FileStorage::open.
		*/
		CV_WRAP FileStorage();

		/** @overload
		@copydoc open()
		*/
		CV_WRAP FileStorage(const String& filename, int flags, const String& encoding = String());

		/** @overload */
		FileStorage(CvFileStorage* fs, bool owning = true);

		//! the destructor. calls release()
		virtual ~FileStorage();

		/** @brief Opens a file.

		See description of parameters in FileStorage::FileStorage. The method calls FileStorage::release
		before opening the file.
		@param filename Name of the file to open or the text string to read the data from.
		Extension of the file (.xml, .yml/.yaml or .json) determines its format (XML, YAML or JSON
		respectively). Also you can append .gz to work with compressed files, for example myHugeMatrix.xml.gz. If both
		FileStorage::WRITE and FileStorage::MEMORY flags are specified, source is used just to specify
		the output file format (e.g. mydata.xml, .yml etc.). A file name can also contain parameters.
		You can use this format, "*?base64" (e.g. "file.json?base64" (case sensitive)), as an alternative to
		FileStorage::BASE64 flag.
		@param flags Mode of operation. One of FileStorage::Mode
		@param encoding Encoding of the file. Note that UTF-16 XML encoding is not supported currently and
		you should use 8-bit encoding instead of it.
		*/
		CV_WRAP virtual bool open(const String& filename, int flags, const String& encoding = String());

		/** @brief Checks whether the file is opened.

		@returns true if the object is associated with the current file and false otherwise. It is a
		good practice to call this method after you tried to open a file.
		*/
		CV_WRAP virtual bool isOpened() const;

		/** @brief Closes the file and releases all the memory buffers.

		Call this method after all I/O operations with the storage are finished.
		*/
		CV_WRAP virtual void release();

		/** @brief Closes the file and releases all the memory buffers.

		Call this method after all I/O operations with the storage are finished. If the storage was
		opened for writing data and FileStorage::WRITE was specified
		*/
		CV_WRAP virtual String releaseAndGetString();

		/** @brief Returns the first element of the top-level mapping.
		@returns The first element of the top-level mapping.
		*/
		CV_WRAP FileNode getFirstTopLevelNode() const;

		/** @brief Returns the top-level mapping
		@param streamidx Zero-based index of the stream. In most cases there is only one stream in the file.
		However, YAML supports multiple streams and so there can be several.
		@returns The top-level mapping.
		*/
		CV_WRAP FileNode root(int streamidx = 0) const;

		/** @brief Returns the specified element of the top-level mapping.
		@param nodename Name of the file node.
		@returns Node with the given name.
		*/
		FileNode operator[](const String& nodename) const;

		/** @overload */
		CV_WRAP_AS(getNode) FileNode operator[](const char* nodename) const;

		/** @brief Returns the obsolete C FileStorage structure.
		@returns Pointer to the underlying C FileStorage structure
		*/
		CvFileStorage* operator *() { return fs.get(); }

		/** @overload */
		const CvFileStorage* operator *() const { return fs.get(); }

		/** @brief Writes multiple numbers.

		Writes one or more numbers of the specified format to the currently written structure. Usually it is
		more convenient to use operator `<<` instead of this method.
		@param fmt Specification of each array element, see @ref format_spec "format specification"
		@param vec Pointer to the written array.
		@param len Number of the uchar elements to write.
		*/
		void writeRaw(const String& fmt, const uchar* vec, size_t len);

		/** @brief Writes the registered C structure (CvMat, CvMatND, CvSeq).
		@param name Name of the written object.
		@param obj Pointer to the object.
		@see ocvWrite for details.
		*/
		void writeObj(const String& name, const void* obj);

		/**
		* @brief Simplified writing API to use with bindings.
		* @param name Name of the written object
		* @param val Value of the written object
		*/
		CV_WRAP void write(const String& name, double val);
		/// @overload
		CV_WRAP void write(const String& name, const String& val);
		/// @overload
		CV_WRAP void write(const String& name, InputArray val);

		/** @brief Writes a comment.

		The function writes a comment into file storage. The comments are skipped when the storage is read.
		@param comment The written comment, single-line or multi-line
		@param append If true, the function tries to put the comment at the end of current line.
		Else if the comment is multi-line, or if it does not fit at the end of the current
		line, the comment starts a new line.
		*/
		CV_WRAP void writeComment(const String& comment, bool append = false);

		/** @brief Returns the normalized object name for the specified name of a file.
		@param filename Name of a file
		@returns The normalized object name.
		*/
		static String getDefaultObjectName(const String& filename);

		/** @brief Returns the current format.
		* @returns The current format, see FileStorage::Mode
		*/
		CV_WRAP int getFormat() const;

		Ptr<CvFileStorage> fs; //!< the underlying C FileStorage structure
		String elname; //!< the currently written element
		std::vector<char> structs; //!< the stack of written structures
		int state; //!< the writer state
	};

	template<> CV_EXPORTS void DefaultDeleter<CvFileStorage>::operator ()(CvFileStorage* obj) const;

	/** @brief File Storage Node class.

	The node is used to store each and every element of the file storage opened for reading. When
	XML/YAML file is read, it is first parsed and stored in the memory as a hierarchical collection of
	nodes. Each node can be a "leaf" that is contain a single number or a string, or be a collection of
	other nodes. There can be named collections (mappings) where each element has a name and it is
	accessed by a name, and ordered collections (sequences) where elements do not have names but rather
	accessed by index. Type of the file node can be determined using FileNode::type method.

	Note that file nodes are only used for navigating file storages opened for reading. When a file
	storage is opened for writing, no data is stored in memory after it is written.
	*/
	class CV_EXPORTS_W_SIMPLE FileNode
	{
	public:
		//! type of the file storage node
		enum Type
		{
			NONE = 0, //!< empty node
			INT = 1, //!< an integer
			REAL = 2, //!< floating-point number
			FLOAT = REAL, //!< synonym or REAL
			STR = 3, //!< text string in UTF-8 encoding
			STRING = STR, //!< synonym for STR
			REF = 4, //!< integer of size size_t. Typically used for storing complex dynamic structures where some elements reference the others
			SEQ = 5, //!< sequence
			MAP = 6, //!< mapping
			TYPE_MASK = 7,
			FLOW = 8,  //!< compact representation of a sequence or mapping. Used only by YAML writer
			USER = 16, //!< a registered object (e.g. a matrix)
			EMPTY = 32, //!< empty structure (sequence or mapping)
			NAMED = 64  //!< the node has a name (i.e. it is element of a mapping)
		};
		/** @brief The constructors.

		These constructors are used to create a default file node, construct it from obsolete structures or
		from the another file node.
		*/
		CV_WRAP FileNode();

		/** @overload
		@param fs Pointer to the obsolete file storage structure.
		@param node File node to be used as initialization for the created file node.
		*/
		FileNode(const CvFileStorage* fs, const CvFileNode* node);

		/** @overload
		@param node File node to be used as initialization for the created file node.
		*/
		FileNode(const FileNode& node);

		/** @brief Returns element of a mapping node or a sequence node.
		@param nodename Name of an element in the mapping node.
		@returns Returns the element with the given identifier.
		*/
		FileNode operator[](const String& nodename) const;

		/** @overload
		@param nodename Name of an element in the mapping node.
		*/
		CV_WRAP_AS(getNode) FileNode operator[](const char* nodename) const;

		/** @overload
		@param i Index of an element in the sequence node.
		*/
		CV_WRAP_AS(at) FileNode operator[](int i) const;

		/** @brief Returns type of the node.
		@returns Type of the node. See FileNode::Type
		*/
		CV_WRAP int type() const;

		//! returns true if the node is empty
		CV_WRAP bool empty() const;
		//! returns true if the node is a "none" object
		CV_WRAP bool isNone() const;
		//! returns true if the node is a sequence
		CV_WRAP bool isSeq() const;
		//! returns true if the node is a mapping
		CV_WRAP bool isMap() const;
		//! returns true if the node is an integer
		CV_WRAP bool isInt() const;
		//! returns true if the node is a floating-point number
		CV_WRAP bool isReal() const;
		//! returns true if the node is a text string
		CV_WRAP bool isString() const;
		//! returns true if the node has a name
		CV_WRAP bool isNamed() const;
		//! returns the node name or an empty string if the node is nameless
		CV_WRAP String name() const;
		//! returns the number of elements in the node, if it is a sequence or mapping, or 1 otherwise.
		CV_WRAP size_t size() const;
		//! returns the node content as an integer. If the node stores floating-point number, it is rounded.
		operator int() const;
		//! returns the node content as float
		operator float() const;
		//! returns the node content as double
		operator double() const;
		//! returns the node content as text string
		operator String() const;
		operator std::string() const;

		//! returns pointer to the underlying file node
		CvFileNode* operator *();
		//! returns pointer to the underlying file node
		const CvFileNode* operator* () const;

		//! returns iterator pointing to the first node element
		FileNodeIterator begin() const;
		//! returns iterator pointing to the element following the last node element
		FileNodeIterator end() const;

		/** @brief Reads node elements to the buffer with the specified format.

		Usually it is more convenient to use operator `>>` instead of this method.
		@param fmt Specification of each array element. See @ref format_spec "format specification"
		@param vec Pointer to the destination array.
		@param len Number of elements to read. If it is greater than number of remaining elements then all
		of them will be read.
		*/
		void readRaw(const String& fmt, uchar* vec, size_t len) const;

		//! reads the registered object and returns pointer to it
		void* readObj() const;

		//! Simplified reading API to use with bindings.
		CV_WRAP double real() const;
		//! Simplified reading API to use with bindings.
		CV_WRAP String string() const;
		//! Simplified reading API to use with bindings.
		CV_WRAP Mat mat() const;

		// do not use wrapper pointer classes for better efficiency
		const CvFileStorage* fs;
		const CvFileNode* node;
	};


	/** @brief used to iterate through sequences and mappings.

	A standard STL notation, with node.begin(), node.end() denoting the beginning and the end of a
	sequence, stored in node. See the data reading sample in the beginning of the section.
	*/
	class CV_EXPORTS FileNodeIterator
	{
	public:
		/** @brief The constructors.

		These constructors are used to create a default iterator, set it to specific element in a file node
		or construct it from another iterator.
		*/
		FileNodeIterator();

		/** @overload
		@param fs File storage for the iterator.
		@param node File node for the iterator.
		@param ofs Index of the element in the node. The created iterator will point to this element.
		*/
		FileNodeIterator(const CvFileStorage* fs, const CvFileNode* node, size_t ofs = 0);

		/** @overload
		@param it Iterator to be used as initialization for the created iterator.
		*/
		FileNodeIterator(const FileNodeIterator& it);

		//! returns the currently observed element
		FileNode operator *() const;
		//! accesses the currently observed element methods
		FileNode operator ->() const;

		//! moves iterator to the next node
		FileNodeIterator& operator ++ ();
		//! moves iterator to the next node
		FileNodeIterator operator ++ (int);
		//! moves iterator to the previous node
		FileNodeIterator& operator -- ();
		//! moves iterator to the previous node
		FileNodeIterator operator -- (int);
		//! moves iterator forward by the specified offset (possibly negative)
		FileNodeIterator& operator += (int ofs);
		//! moves iterator backward by the specified offset (possibly negative)
		FileNodeIterator& operator -= (int ofs);

		/** @brief Reads node elements to the buffer with the specified format.

		Usually it is more convenient to use operator `>>` instead of this method.
		@param fmt Specification of each array element. See @ref format_spec "format specification"
		@param vec Pointer to the destination array.
		@param maxCount Number of elements to read. If it is greater than number of remaining elements then
		all of them will be read.
		*/
		FileNodeIterator& readRaw(const String& fmt, uchar* vec,
			size_t maxCount = (size_t)INT_MAX);

		struct SeqReader
		{
			int          header_size;
			void*        seq;        /* sequence, beign read; CvSeq      */
			void*        block;      /* current block;        CvSeqBlock */
			schar*       ptr;        /* pointer to element be read next */
			schar*       block_min;  /* pointer to the beginning of block */
			schar*       block_max;  /* pointer to the end of block */
			int          delta_index;/* = seq->first->start_index   */
			schar*       prev_elem;  /* pointer to previous element */
		};

		const CvFileStorage* fs;
		const CvFileNode* container;
		SeqReader reader;
		size_t remaining;
	};

	//! @} core_xml

	/////////////////// XML & YAML I/O implementation //////////////////

	//! @relates cv::FileStorage
	//! @{

	CV_EXPORTS void write(FileStorage& fs, const String& name, int value);
	CV_EXPORTS void write(FileStorage& fs, const String& name, float value);
	CV_EXPORTS void write(FileStorage& fs, const String& name, double value);
	CV_EXPORTS void write(FileStorage& fs, const String& name, const String& value);
	CV_EXPORTS void write(FileStorage& fs, const String& name, const Mat& value);
	CV_EXPORTS void write(FileStorage& fs, const String& name, const SparseMat& value);
#ifdef CV__LEGACY_PERSISTENCE
	CV_EXPORTS void write(FileStorage& fs, const String& name, const std::vector<KeyPoint>& value);
	CV_EXPORTS void write(FileStorage& fs, const String& name, const std::vector<DMatch>& value);
#endif

	CV_EXPORTS void writeScalar(FileStorage& fs, int value);
	CV_EXPORTS void writeScalar(FileStorage& fs, float value);
	CV_EXPORTS void writeScalar(FileStorage& fs, double value);
	CV_EXPORTS void writeScalar(FileStorage& fs, const String& value);

	//! @}

	//! @relates cv::FileNode
	//! @{

	CV_EXPORTS void read(const FileNode& node, int& value, int default_value);
	CV_EXPORTS void read(const FileNode& node, float& value, float default_value);
	CV_EXPORTS void read(const FileNode& node, double& value, double default_value);
	CV_EXPORTS void read(const FileNode& node, String& value, const String& default_value);
	CV_EXPORTS void read(const FileNode& node, std::string& value, const std::string& default_value);
	CV_EXPORTS void read(const FileNode& node, Mat& mat, const Mat& default_mat = Mat());
	CV_EXPORTS void read(const FileNode& node, SparseMat& mat, const SparseMat& default_mat = SparseMat());
#ifdef CV__LEGACY_PERSISTENCE
	CV_EXPORTS void read(const FileNode& node, std::vector<KeyPoint>& keypoints);
	CV_EXPORTS void read(const FileNode& node, std::vector<DMatch>& matches);
#endif
	CV_EXPORTS void read(const FileNode& node, KeyPoint& value, const KeyPoint& default_value);
	CV_EXPORTS void read(const FileNode& node, DMatch& value, const DMatch& default_value);

	template<typename _Tp> static inline void read(const FileNode& node, Point_<_Tp>& value, const Point_<_Tp>& default_value)
	{
		std::vector<_Tp> temp; FileNodeIterator it = node.begin(); it >> temp;
		value = temp.size() != 2 ? default_value : Point_<_Tp>(saturate_cast<_Tp>(temp[0]), saturate_cast<_Tp>(temp[1]));
	}

	template<typename _Tp> static inline void read(const FileNode& node, Point3_<_Tp>& value, const Point3_<_Tp>& default_value)
	{
		std::vector<_Tp> temp; FileNodeIterator it = node.begin(); it >> temp;
		value = temp.size() != 3 ? default_value : Point3_<_Tp>(saturate_cast<_Tp>(temp[0]), saturate_cast<_Tp>(temp[1]),
			saturate_cast<_Tp>(temp[2]));
	}

	template<typename _Tp> static inline void read(const FileNode& node, Size_<_Tp>& value, const Size_<_Tp>& default_value)
	{
		std::vector<_Tp> temp; FileNodeIterator it = node.begin(); it >> temp;
		value = temp.size() != 2 ? default_value : Size_<_Tp>(saturate_cast<_Tp>(temp[0]), saturate_cast<_Tp>(temp[1]));
	}

	template<typename _Tp> static inline void read(const FileNode& node, Complex<_Tp>& value, const Complex<_Tp>& default_value)
	{
		std::vector<_Tp> temp; FileNodeIterator it = node.begin(); it >> temp;
		value = temp.size() != 2 ? default_value : Complex<_Tp>(saturate_cast<_Tp>(temp[0]), saturate_cast<_Tp>(temp[1]));
	}

	template<typename _Tp> static inline void read(const FileNode& node, Rect_<_Tp>& value, const Rect_<_Tp>& default_value)
	{
		std::vector<_Tp> temp; FileNodeIterator it = node.begin(); it >> temp;
		value = temp.size() != 4 ? default_value : Rect_<_Tp>(saturate_cast<_Tp>(temp[0]), saturate_cast<_Tp>(temp[1]),
			saturate_cast<_Tp>(temp[2]), saturate_cast<_Tp>(temp[3]));
	}

	template<typename _Tp, int cn> static inline void read(const FileNode& node, Vec<_Tp, cn>& value, const Vec<_Tp, cn>& default_value)
	{
		std::vector<_Tp> temp; FileNodeIterator it = node.begin(); it >> temp;
		value = temp.size() != cn ? default_value : Vec<_Tp, cn>(&temp[0]);
	}

	template<typename _Tp> static inline void read(const FileNode& node, Scalar_<_Tp>& value, const Scalar_<_Tp>& default_value)
	{
		std::vector<_Tp> temp; FileNodeIterator it = node.begin(); it >> temp;
		value = temp.size() != 4 ? default_value : Scalar_<_Tp>(saturate_cast<_Tp>(temp[0]), saturate_cast<_Tp>(temp[1]),
			saturate_cast<_Tp>(temp[2]), saturate_cast<_Tp>(temp[3]));
	}

	static inline void read(const FileNode& node, Range& value, const Range& default_value)
	{
		Point2i temp(value.start, value.end); const Point2i default_temp = Point2i(default_value.start, default_value.end);
		read(node, temp, default_temp);
		value.start = temp.x; value.end = temp.y;
	}

	//! @}

	/** @brief Writes string to a file storage.
	@relates cv::FileStorage
	*/
	CV_EXPORTS FileStorage& operator << (FileStorage& fs, const String& str);

	//! @cond IGNORED

	namespace internal
	{
		class CV_EXPORTS WriteStructContext
		{
		public:
			WriteStructContext(FileStorage& _fs, const String& name, int flags, const String& typeName = String());
			~WriteStructContext();
		private:
			FileStorage * fs;
		};

		template<typename _Tp, int numflag> class VecWriterProxy
		{
		public:
			VecWriterProxy(FileStorage* _fs) : fs(_fs) {}
			void operator()(const std::vector<_Tp>& vec) const
			{
				size_t count = vec.size();
				for (size_t i = 0; i < count; i++)
					write(*fs, vec[i]);
			}
		private:
			FileStorage * fs;
		};

		template<typename _Tp> class VecWriterProxy<_Tp, 1>
		{
		public:
			VecWriterProxy(FileStorage* _fs) : fs(_fs) {}
			void operator()(const std::vector<_Tp>& vec) const
			{
				int _fmt = traits::SafeFmt<_Tp>::fmt;
				char fmt[] = { (char)((_fmt >> 8) + '1'), (char)_fmt, '\0' };
				fs->writeRaw(fmt, !vec.empty() ? (uchar*)&vec[0] : 0, vec.size() * sizeof(_Tp));
			}
		private:
			FileStorage * fs;
		};

		template<typename _Tp, int numflag> class VecReaderProxy
		{
		public:
			VecReaderProxy(FileNodeIterator* _it) : it(_it) {}
			void operator()(std::vector<_Tp>& vec, size_t count) const
			{
				count = std::min(count, it->remaining);
				vec.resize(count);
				for (size_t i = 0; i < count; i++, ++(*it))
					read(**it, vec[i], _Tp());
			}
		private:
			FileNodeIterator * it;
		};

		template<typename _Tp> class VecReaderProxy<_Tp, 1>
		{
		public:
			VecReaderProxy(FileNodeIterator* _it) : it(_it) {}
			void operator()(std::vector<_Tp>& vec, size_t count) const
			{
				size_t remaining = it->remaining;
				size_t cn = DataType<_Tp>::channels;
				int _fmt = traits::SafeFmt<_Tp>::fmt;
				CV_Assert((_fmt >> 8) < 9);
				char fmt[] = { (char)((_fmt >> 8) + '1'), (char)_fmt, '\0' };
				CV_Assert((remaining % cn) == 0);
				size_t remaining1 = remaining / cn;
				count = count < remaining1 ? count : remaining1;
				vec.resize(count);
				it->readRaw(fmt, !vec.empty() ? (uchar*)&vec[0] : 0, count * sizeof(_Tp));
			}
		private:
			FileNodeIterator * it;
		};

	} // internal

	  //! @endcond

	  //! @relates cv::FileStorage
	  //! @{

	template<typename _Tp> static inline
		void write(FileStorage& fs, const _Tp& value)
	{
		write(fs, String(), value);
	}

	template<> inline
		void write(FileStorage& fs, const int& value)
	{
		writeScalar(fs, value);
	}

	template<> inline
		void write(FileStorage& fs, const float& value)
	{
		writeScalar(fs, value);
	}

	template<> inline
		void write(FileStorage& fs, const double& value)
	{
		writeScalar(fs, value);
	}

	template<> inline
		void write(FileStorage& fs, const String& value)
	{
		writeScalar(fs, value);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const Point_<_Tp>& pt)
	{
		write(fs, pt.x);
		write(fs, pt.y);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const Point3_<_Tp>& pt)
	{
		write(fs, pt.x);
		write(fs, pt.y);
		write(fs, pt.z);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const Size_<_Tp>& sz)
	{
		write(fs, sz.width);
		write(fs, sz.height);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const Complex<_Tp>& c)
	{
		write(fs, c.re);
		write(fs, c.im);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const Rect_<_Tp>& r)
	{
		write(fs, r.x);
		write(fs, r.y);
		write(fs, r.width);
		write(fs, r.height);
	}

	template<typename _Tp, int cn> static inline
		void write(FileStorage& fs, const Vec<_Tp, cn>& v)
	{
		for (int i = 0; i < cn; i++)
			write(fs, v.val[i]);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const Scalar_<_Tp>& s)
	{
		write(fs, s.val[0]);
		write(fs, s.val[1]);
		write(fs, s.val[2]);
		write(fs, s.val[3]);
	}

	static inline
		void write(FileStorage& fs, const Range& r)
	{
		write(fs, r.start);
		write(fs, r.end);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const std::vector<_Tp>& vec)
	{
		cv::internal::VecWriterProxy<_Tp, traits::SafeFmt<_Tp>::fmt != 0> w(&fs);
		w(vec);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const String& name, const Point_<_Tp>& pt)
	{
		cv::internal::WriteStructContext ws(fs, name, FileNode::SEQ + FileNode::FLOW);
		write(fs, pt);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const String& name, const Point3_<_Tp>& pt)
	{
		cv::internal::WriteStructContext ws(fs, name, FileNode::SEQ + FileNode::FLOW);
		write(fs, pt);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const String& name, const Size_<_Tp>& sz)
	{
		cv::internal::WriteStructContext ws(fs, name, FileNode::SEQ + FileNode::FLOW);
		write(fs, sz);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const String& name, const Complex<_Tp>& c)
	{
		cv::internal::WriteStructContext ws(fs, name, FileNode::SEQ + FileNode::FLOW);
		write(fs, c);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const String& name, const Rect_<_Tp>& r)
	{
		cv::internal::WriteStructContext ws(fs, name, FileNode::SEQ + FileNode::FLOW);
		write(fs, r);
	}

	template<typename _Tp, int cn> static inline
		void write(FileStorage& fs, const String& name, const Vec<_Tp, cn>& v)
	{
		cv::internal::WriteStructContext ws(fs, name, FileNode::SEQ + FileNode::FLOW);
		write(fs, v);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const String& name, const Scalar_<_Tp>& s)
	{
		cv::internal::WriteStructContext ws(fs, name, FileNode::SEQ + FileNode::FLOW);
		write(fs, s);
	}

	static inline
		void write(FileStorage& fs, const String& name, const Range& r)
	{
		cv::internal::WriteStructContext ws(fs, name, FileNode::SEQ + FileNode::FLOW);
		write(fs, r);
	}

	static inline
		void write(FileStorage& fs, const String& name, const KeyPoint& kpt)
	{
		cv::internal::WriteStructContext ws(fs, name, FileNode::SEQ + FileNode::FLOW);
		write(fs, kpt.pt.x);
		write(fs, kpt.pt.y);
		write(fs, kpt.size);
		write(fs, kpt.angle);
		write(fs, kpt.response);
		write(fs, kpt.octave);
		write(fs, kpt.class_id);
	}

	static inline
		void write(FileStorage& fs, const String& name, const DMatch& m)
	{
		cv::internal::WriteStructContext ws(fs, name, FileNode::SEQ + FileNode::FLOW);
		write(fs, m.queryIdx);
		write(fs, m.trainIdx);
		write(fs, m.imgIdx);
		write(fs, m.distance);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const String& name, const std::vector<_Tp>& vec)
	{
		cv::internal::WriteStructContext ws(fs, name, FileNode::SEQ + (traits::SafeFmt<_Tp>::fmt != 0 ? FileNode::FLOW : 0));
		write(fs, vec);
	}

	template<typename _Tp> static inline
		void write(FileStorage& fs, const String& name, const std::vector< std::vector<_Tp> >& vec)
	{
		cv::internal::WriteStructContext ws(fs, name, FileNode::SEQ);
		for (size_t i = 0; i < vec.size(); i++)
		{
			cv::internal::WriteStructContext ws_(fs, name, FileNode::SEQ + (traits::SafeFmt<_Tp>::fmt != 0 ? FileNode::FLOW : 0));
			write(fs, vec[i]);
		}
	}

#ifdef CV__LEGACY_PERSISTENCE
	// This code is not needed anymore, but it is preserved here to keep source compatibility
	// Implementation is similar to templates instantiations
	static inline void write(FileStorage& fs, const KeyPoint& kpt) { write(fs, String(), kpt); }
	static inline void write(FileStorage& fs, const DMatch& m) { write(fs, String(), m); }
	static inline void write(FileStorage& fs, const std::vector<KeyPoint>& vec)
	{
		cv::internal::VecWriterProxy<KeyPoint, 0> w(&fs);
		w(vec);
	}
	static inline void write(FileStorage& fs, const std::vector<DMatch>& vec)
	{
		cv::internal::VecWriterProxy<DMatch, 0> w(&fs);
		w(vec);

	}
#endif

	//! @} FileStorage

	//! @relates cv::FileNode
	//! @{

	static inline
		void read(const FileNode& node, bool& value, bool default_value)
	{
		int temp;
		read(node, temp, (int)default_value);
		value = temp != 0;
	}

	static inline
		void read(const FileNode& node, uchar& value, uchar default_value)
	{
		int temp;
		read(node, temp, (int)default_value);
		value = saturate_cast<uchar>(temp);
	}

	static inline
		void read(const FileNode& node, schar& value, schar default_value)
	{
		int temp;
		read(node, temp, (int)default_value);
		value = saturate_cast<schar>(temp);
	}

	static inline
		void read(const FileNode& node, ushort& value, ushort default_value)
	{
		int temp;
		read(node, temp, (int)default_value);
		value = saturate_cast<ushort>(temp);
	}

	static inline
		void read(const FileNode& node, short& value, short default_value)
	{
		int temp;
		read(node, temp, (int)default_value);
		value = saturate_cast<short>(temp);
	}

	template<typename _Tp> static inline
		void read(FileNodeIterator& it, std::vector<_Tp>& vec, size_t maxCount = (size_t)INT_MAX)
	{
		cv::internal::VecReaderProxy<_Tp, traits::SafeFmt<_Tp>::fmt != 0> r(&it);
		r(vec, maxCount);
	}

	template<typename _Tp> static inline
		void read(const FileNode& node, std::vector<_Tp>& vec, const std::vector<_Tp>& default_value = std::vector<_Tp>())
	{
		if (!node.node)
			vec = default_value;
		else
		{
			FileNodeIterator it = node.begin();
			read(it, vec);
		}
	}

	static inline
		void read(const FileNode& node, std::vector<KeyPoint>& vec, const std::vector<KeyPoint>& default_value)
	{
		if (!node.node)
			vec = default_value;
		else
			read(node, vec);
	}

	static inline
		void read(const FileNode& node, std::vector<DMatch>& vec, const std::vector<DMatch>& default_value)
	{
		if (!node.node)
			vec = default_value;
		else
			read(node, vec);
	}

	//! @} FileNode

	//! @relates cv::FileStorage
	//! @{

	/** @brief Writes data to a file storage.
	*/
	template<typename _Tp> static inline
		FileStorage& operator << (FileStorage& fs, const _Tp& value)
	{
		if (!fs.isOpened())
			return fs;
		if (fs.state == FileStorage::NAME_EXPECTED + FileStorage::INSIDE_MAP)
			CV_Error(Error::StsError, "No element name has been given");
		write(fs, fs.elname, value);
		if (fs.state & FileStorage::INSIDE_MAP)
			fs.state = FileStorage::NAME_EXPECTED + FileStorage::INSIDE_MAP;
		return fs;
	}

	/** @brief Writes data to a file storage.
	*/
	static inline
		FileStorage& operator << (FileStorage& fs, const char* str)
	{
		return (fs << String(str));
	}

	/** @brief Writes data to a file storage.
	*/
	static inline
		FileStorage& operator << (FileStorage& fs, char* value)
	{
		return (fs << String(value));
	}

	//! @} FileStorage

	//! @relates cv::FileNodeIterator
	//! @{

	/** @brief Reads data from a file storage.
	*/
	template<typename _Tp> static inline
		FileNodeIterator& operator >> (FileNodeIterator& it, _Tp& value)
	{
		read(*it, value, _Tp());
		return ++it;
	}

	/** @brief Reads data from a file storage.
	*/
	template<typename _Tp> static inline
		FileNodeIterator& operator >> (FileNodeIterator& it, std::vector<_Tp>& vec)
	{
		cv::internal::VecReaderProxy<_Tp, traits::SafeFmt<_Tp>::fmt != 0> r(&it);
		r(vec, (size_t)INT_MAX);
		return it;
	}

	//! @} FileNodeIterator

	//! @relates cv::FileNode
	//! @{

	/** @brief Reads data from a file storage.
	*/
	template<typename _Tp> static inline
		void operator >> (const FileNode& n, _Tp& value)
	{
		read(n, value, _Tp());
	}

	/** @brief Reads data from a file storage.
	*/
	template<typename _Tp> static inline
		void operator >> (const FileNode& n, std::vector<_Tp>& vec)
	{
		FileNodeIterator it = n.begin();
		it >> vec;
	}

	/** @brief Reads KeyPoint from a file storage.
	*/
	//It needs special handling because it contains two types of fields, int & float.
	static inline
		void operator >> (const FileNode& n, KeyPoint& kpt)
	{
		FileNodeIterator it = n.begin();
		it >> kpt.pt.x >> kpt.pt.y >> kpt.size >> kpt.angle >> kpt.response >> kpt.octave >> kpt.class_id;
	}

#ifdef CV__LEGACY_PERSISTENCE
	static inline
		void operator >> (const FileNode& n, std::vector<KeyPoint>& vec)
	{
		read(n, vec);
	}
	static inline
		void operator >> (const FileNode& n, std::vector<DMatch>& vec)
	{
		read(n, vec);
	}
#endif

	/** @brief Reads DMatch from a file storage.
	*/
	//It needs special handling because it contains two types of fields, int & float.
	static inline
		void operator >> (const FileNode& n, DMatch& m)
	{
		FileNodeIterator it = n.begin();
		it >> m.queryIdx >> m.trainIdx >> m.imgIdx >> m.distance;
	}

	//! @} FileNode

	//! @relates cv::FileNodeIterator
	//! @{

	static inline
		bool operator == (const FileNodeIterator& it1, const FileNodeIterator& it2)
	{
		return it1.fs == it2.fs && it1.container == it2.container &&
			it1.reader.ptr == it2.reader.ptr && it1.remaining == it2.remaining;
	}

	static inline
		bool operator != (const FileNodeIterator& it1, const FileNodeIterator& it2)
	{
		return !(it1 == it2);
	}

	static inline
		ptrdiff_t operator - (const FileNodeIterator& it1, const FileNodeIterator& it2)
	{
		return it2.remaining - it1.remaining;
	}

	static inline
		bool operator < (const FileNodeIterator& it1, const FileNodeIterator& it2)
	{
		return it1.remaining > it2.remaining;
	}

	//! @} FileNodeIterator

	//! @cond IGNORED

	inline FileNode FileStorage::getFirstTopLevelNode() const { FileNode r = root(); FileNodeIterator it = r.begin(); return it != r.end() ? *it : FileNode(); }
	inline FileNode::FileNode() : fs(0), node(0) {}
	inline FileNode::FileNode(const CvFileStorage* _fs, const CvFileNode* _node) : fs(_fs), node(_node) {}
	inline FileNode::FileNode(const FileNode& _node) : fs(_node.fs), node(_node.node) {}
	inline bool FileNode::empty() const { return node == 0; }
	inline bool FileNode::isNone() const { return type() == NONE; }
	inline bool FileNode::isSeq() const { return type() == SEQ; }
	inline bool FileNode::isMap() const { return type() == MAP; }
	inline bool FileNode::isInt() const { return type() == INT; }
	inline bool FileNode::isReal() const { return type() == REAL; }
	inline bool FileNode::isString() const { return type() == STR; }
	inline CvFileNode* FileNode::operator *() { return (CvFileNode*)node; }
	inline const CvFileNode* FileNode::operator* () const { return node; }
	inline FileNode::operator int() const { int value;    read(*this, value, 0);     return value; }
	inline FileNode::operator float() const { float value;  read(*this, value, 0.f);   return value; }
	inline FileNode::operator double() const { double value; read(*this, value, 0.);    return value; }
	inline FileNode::operator String() const { String value; read(*this, value, value); return value; }
	inline double FileNode::real() const { return double(*this); }
	inline String FileNode::string() const { return String(*this); }
	inline Mat FileNode::mat() const { Mat value; read(*this, value, value);    return value; }
	inline FileNodeIterator FileNode::begin() const { return FileNodeIterator(fs, node); }
	inline FileNodeIterator FileNode::end() const { return FileNodeIterator(fs, node, size()); }
	inline void FileNode::readRaw(const String& fmt, uchar* vec, size_t len) const { begin().readRaw(fmt, vec, len); }
	inline FileNode FileNodeIterator::operator *() const { return FileNode(fs, (const CvFileNode*)(const void*)reader.ptr); }
	inline FileNode FileNodeIterator::operator ->() const { return FileNode(fs, (const CvFileNode*)(const void*)reader.ptr); }
	inline String::String(const FileNode& fn) : cstr_(0), len_(0) { read(fn, *this, *this); }

	//! @endcond


	CV_EXPORTS void cvStartWriteRawData_Base64(::CvFileStorage * fs, const char* name, int len, const char* dt);

	CV_EXPORTS void cvWriteRawData_Base64(::CvFileStorage * fs, const void* _data, int len);

	CV_EXPORTS void cvEndWriteRawData_Base64(::CvFileStorage * fs);

	CV_EXPORTS void cvWriteMat_Base64(::CvFileStorage* fs, const char* name, const ::CvMat* mat);

	CV_EXPORTS void cvWriteMatND_Base64(::CvFileStorage* fs, const char* name, const ::CvMatND* mat);

} // cv

#endif // OPENCV_CORE_PERSISTENCE_HPP
