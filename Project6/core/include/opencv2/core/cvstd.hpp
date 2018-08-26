#ifndef OPENCV_CORE_CVSTD_HPP
#define OPENCV_CORE_CVSTD_HPP

#include "cvdef.h"
#include <cstddef>
#include <cstring>
#include <cctype>
#include <string>

#include <algorithm>
#include <utility>
#include <cstdlib> //for abs(int)
#include <cmath>

namespace cv
{
	static inline uchar abs(uchar a) { return a; }
	static inline ushort abs(ushort a) { return a; }
	static inline unsigned abs(unsigned a) { return a; }
	static inline uint64 abs(uint64 a) { return a; }

	using std::min;
	using std::max;
	using std::abs;
	using std::swap;
	using std::sqrt;
	using std::exp;
	using std::pow;
	using std::log;
}

namespace cv
{
	template<typename T>
	struct Ptr
	{
		/** Generic programming support. */
		typedef T element_type;

		/** The default constructor creates a null Ptr - one that owns and stores a null pointer.
		*/
		Ptr();

		/**
		If p is null, these are equivalent to the default constructor.
		Otherwise, these constructors assume ownership of p - that is, the created Ptr owns and stores p
		and assumes it is the sole owner of it. Don't use them if p is already owned by another Ptr, or
		else p will get deleted twice.
		With the first constructor, DefaultDeleter\<Y\>() becomes the associated deleter (so p will
		eventually be deleted with the standard delete operator). Y must be a complete type at the point
		of invocation.
		With the second constructor, d becomes the associated deleter.
		Y\* must be convertible to T\*.
		@param p Pointer to own.
		@note It is often easier to use makePtr instead.
		*/
		template<typename Y>
#ifdef DISABLE_OPENCV_24_COMPATIBILITY
		explicit
#endif
			Ptr(Y* p);

		/** @overload
		@param d Deleter to use for the owned pointer.
		@param p Pointer to own.
		*/
		template<typename Y, typename D>
		Ptr(Y* p, D d);

		/**
		These constructors create a Ptr that shares ownership with another Ptr - that is, own the same
		pointer as o.
		With the first two, the same pointer is stored, as well; for the second, Y\* must be convertible
		to T\*.
		With the third, p is stored, and Y may be any type. This constructor allows to have completely
		unrelated owned and stored pointers, and should be used with care to avoid confusion. A relatively
		benign use is to create a non-owning Ptr, like this:
		@code
		ptr = Ptr<T>(Ptr<T>(), dont_delete_me); // owns nothing; will not delete the pointer.
		@endcode
		@param o Ptr to share ownership with.
		*/
		Ptr(const Ptr& o);

		/** @overload
		@param o Ptr to share ownership with.
		*/
		template<typename Y>
		Ptr(const Ptr<Y>& o);

		/** @overload
		@param o Ptr to share ownership with.
		@param p Pointer to store.
		*/
		template<typename Y>
		Ptr(const Ptr<Y>& o, T* p);

		/** The destructor is equivalent to calling Ptr::release. */
		~Ptr();

		/**
		Assignment replaces the current Ptr instance with one that owns and stores same pointers as o and
		then destroys the old instance.
		@param o Ptr to share ownership with.
		*/
		Ptr& operator = (const Ptr& o);

		/** @overload */
		template<typename Y>
		Ptr& operator = (const Ptr<Y>& o);

		/** If no other Ptr instance owns the owned pointer, deletes it with the associated deleter. Then sets
		both the owned and the stored pointers to NULL.
		*/
		void release();

		/**
		`ptr.reset(...)` is equivalent to `ptr = Ptr<T>(...)`.
		@param p Pointer to own.
		*/
		template<typename Y>
		void reset(Y* p);

		/** @overload
		@param d Deleter to use for the owned pointer.
		@param p Pointer to own.
		*/
		template<typename Y, typename D>
		void reset(Y* p, D d);

		/**
		Swaps the owned and stored pointers (and deleters, if any) of this and o.
		@param o Ptr to swap with.
		*/
		void swap(Ptr& o);

		/** Returns the stored pointer. */
		T* get() const;

		/** Ordinary pointer emulation. */
		typename detail::RefOrVoid<T>::type operator * () const;

		/** Ordinary pointer emulation. */
		T* operator -> () const;

		/** Equivalent to get(). */
		operator T* () const;

		/** ptr.empty() is equivalent to `!ptr.get()`. */
		bool empty() const;

		/** Returns a Ptr that owns the same pointer as this, and stores the same
		pointer as this, except converted via static_cast to Y*.
		*/
		template<typename Y>
		Ptr<Y> staticCast() const;

		/** Ditto for const_cast. */
		template<typename Y>
		Ptr<Y> constCast() const;

		/** Ditto for dynamic_cast. */
		template<typename Y>
		Ptr<Y> dynamicCast() const;

#ifdef CV_CXX_MOVE_SEMANTICS
		Ptr(Ptr&& o);
		Ptr& operator = (Ptr&& o);
#endif

	private:
		detail::PtrOwner* owner;
		T* stored;

		template<typename Y>
		friend struct Ptr; // have to do this for the cross-type copy constructor
	};

	/** Equivalent to ptr1.swap(ptr2). Provided to help write generic algorithms. */
	template<typename T>
	void swap(Ptr<T>& ptr1, Ptr<T>& ptr2);

	/** Return whether ptr1.get() and ptr2.get() are equal and not equal, respectively. */
	template<typename T>
	bool operator == (const Ptr<T>& ptr1, const Ptr<T>& ptr2);
	template<typename T>
	bool operator != (const Ptr<T>& ptr1, const Ptr<T>& ptr2);

	/** `makePtr<T>(...)` is equivalent to `Ptr<T>(new T(...))`. It is shorter than the latter, and it's
	marginally safer than using a constructor or Ptr::reset, since it ensures that the owned pointer
	is new and thus not owned by any other Ptr instance.
	Unfortunately, perfect forwarding is impossible to implement in C++03, and so makePtr is limited
	to constructors of T that have up to 10 arguments, none of which are non-const references.
	*/
	template<typename T>
	Ptr<T> makePtr();
	/** @overload */
	template<typename T, typename A1>
	Ptr<T> makePtr(const A1& a1);
	/** @overload */
	template<typename T, typename A1, typename A2>
	Ptr<T> makePtr(const A1& a1, const A2& a2);
	/** @overload */
	template<typename T, typename A1, typename A2, typename A3>
	Ptr<T> makePtr(const A1& a1, const A2& a2, const A3& a3);
	/** @overload */
	template<typename T, typename A1, typename A2, typename A3, typename A4>
	Ptr<T> makePtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4);
	/** @overload */
	template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
	Ptr<T> makePtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5);
	/** @overload */
	template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	Ptr<T> makePtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6);
	/** @overload */
	template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	Ptr<T> makePtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7);
	/** @overload */
	template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	Ptr<T> makePtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8);
	/** @overload */
	template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	Ptr<T> makePtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9);
	/** @overload */
	template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	Ptr<T> makePtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10);

	//////////////////////////////// string class ////////////////////////////////

	class CV_EXPORTS FileNode; //for string constructor from FileNode

	class CV_EXPORTS String
	{
	public:
		typedef char value_type;
		typedef char& reference;
		typedef const char& const_reference;
		typedef char* pointer;
		typedef const char* const_pointer;
		typedef ptrdiff_t difference_type;
		typedef size_t size_type;
		typedef char* iterator;
		typedef const char* const_iterator;

		static const size_t npos = size_t(-1);

		String();
		String(const String& str);
		String(const String& str, size_t pos, size_t len = npos);
		String(const char* s);
		String(const char* s, size_t n);
		String(size_t n, char c);
		String(const char* first, const char* last);
		template<typename Iterator> String(Iterator first, Iterator last);
		explicit String(const FileNode& fn);
		~String();

		String& operator=(const String& str);
		String& operator=(const char* s);
		String& operator=(char c);

		String& operator+=(const String& str);
		String& operator+=(const char* s);
		String& operator+=(char c);

		size_t size() const;
		size_t length() const;

		char operator[](size_t idx) const;
		char operator[](int idx) const;

		const char* begin() const;
		const char* end() const;

		const char* c_str() const;

		bool empty() const;
		void clear();

		int compare(const char* s) const;
		int compare(const String& str) const;

		void swap(String& str);
		String substr(size_t pos = 0, size_t len = npos) const;

		size_t find(const char* s, size_t pos, size_t n) const;
		size_t find(char c, size_t pos = 0) const;
		size_t find(const String& str, size_t pos = 0) const;
		size_t find(const char* s, size_t pos = 0) const;

		size_t rfind(const char* s, size_t pos, size_t n) const;
		size_t rfind(char c, size_t pos = npos) const;
		size_t rfind(const String& str, size_t pos = npos) const;
		size_t rfind(const char* s, size_t pos = npos) const;

		size_t find_first_of(const char* s, size_t pos, size_t n) const;
		size_t find_first_of(char c, size_t pos = 0) const;
		size_t find_first_of(const String& str, size_t pos = 0) const;
		size_t find_first_of(const char* s, size_t pos = 0) const;

		size_t find_last_of(const char* s, size_t pos, size_t n) const;
		size_t find_last_of(char c, size_t pos = npos) const;
		size_t find_last_of(const String& str, size_t pos = npos) const;
		size_t find_last_of(const char* s, size_t pos = npos) const;

		friend String operator+ (const String& lhs, const String& rhs);
		friend String operator+ (const String& lhs, const char*   rhs);
		friend String operator+ (const char*   lhs, const String& rhs);
		friend String operator+ (const String& lhs, char          rhs);
		friend String operator+ (char          lhs, const String& rhs);

		String toLowerCase() const;

		String(const std::string& str);
		String(const std::string& str, size_t pos, size_t len = npos);
		String& operator=(const std::string& str);
		String& operator+=(const std::string& str);
		operator std::string() const;

		friend String operator+ (const String& lhs, const std::string& rhs);
		friend String operator+ (const std::string& lhs, const String& rhs);

	private:
		char*  cstr_;
		size_t len_;

		char* allocate(size_t len); // len without trailing 0
		void deallocate();

		String(int); // disabled and invalid. Catch invalid usages like, commandLineParser.has(0) problem
	};

}

#include "ptr.inl.hpp"



#endif
