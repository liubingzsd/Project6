#include "precomp.hpp"
//#include "opencl_kernels_core.hpp"
//#include "stat.hpp"

/****************************************************************************************\
*                                         norm                                           *
\****************************************************************************************/

namespace cv {
	namespace hal {

		extern const uchar popCountTable[256] =
		{
			0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
			1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
			1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
			2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
			1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
			2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
			2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
			3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
		};

		static const uchar popCountTable2[] =
		{
			0, 1, 1, 1, 1, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3,
			1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3,
			1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4,
			2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4,
			1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4,
			2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4,
			1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4,
			2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4
		};

		static const uchar popCountTable4[] =
		{
			0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
		};


		int normHamming(const uchar* a, int n, int cellSize)
		{
			if (cellSize == 1)
				return normHamming(a, n);
			const uchar* tab = 0;
			if (cellSize == 2)
				tab = popCountTable2;
			else if (cellSize == 4)
				tab = popCountTable4;
			else
				return -1;
			int i = 0;
			int result = 0;
#if CV_ENABLE_UNROLLED
			for (; i <= n - 4; i += 4)
				result += tab[a[i]] + tab[a[i + 1]] + tab[a[i + 2]] + tab[a[i + 3]];
#endif
			for (; i < n; i++)
				result += tab[a[i]];
			return result;
		}

		int normHamming(const uchar* a, const uchar* b, int n, int cellSize)
		{
			if (cellSize == 1)
				return normHamming(a, b, n);
			const uchar* tab = 0;
			if (cellSize == 2)
				tab = popCountTable2;
			else if (cellSize == 4)
				tab = popCountTable4;
			else
				return -1;
			int i = 0;
			int result = 0;
#if CV_ENABLE_UNROLLED
			for (; i <= n - 4; i += 4)
				result += tab[a[i] ^ b[i]] + tab[a[i + 1] ^ b[i + 1]] +
				tab[a[i + 2] ^ b[i + 2]] + tab[a[i + 3] ^ b[i + 3]];
#endif
			for (; i < n; i++)
				result += tab[a[i] ^ b[i]];
			return result;
		}

		float normL2Sqr_(const float* a, const float* b, int n)
		{
			int j = 0; float d = 0.f;

			{
				for (; j <= n - 4; j += 4)
				{
					float t0 = a[j] - b[j], t1 = a[j + 1] - b[j + 1], t2 = a[j + 2] - b[j + 2], t3 = a[j + 3] - b[j + 3];
					d += t0 * t0 + t1 * t1 + t2 * t2 + t3 * t3;
				}
			}

			for (; j < n; j++)
			{
				float t = a[j] - b[j];
				d += t * t;
			}
			return d;
		}


		float normL1_(const float* a, const float* b, int n)
		{
			int j = 0; float d = 0.f;
#if CV_SSE
			float CV_DECL_ALIGNED(16) buf[4];
			static const int CV_DECL_ALIGNED(16) absbuf[4] = { 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff };
			__m128 d0 = _mm_setzero_ps(), d1 = _mm_setzero_ps();
			__m128 absmask = _mm_load_ps((const float*)absbuf);

			for (; j <= n - 8; j += 8)
			{
				__m128 t0 = _mm_sub_ps(_mm_loadu_ps(a + j), _mm_loadu_ps(b + j));
				__m128 t1 = _mm_sub_ps(_mm_loadu_ps(a + j + 4), _mm_loadu_ps(b + j + 4));
				d0 = _mm_add_ps(d0, _mm_and_ps(t0, absmask));
				d1 = _mm_add_ps(d1, _mm_and_ps(t1, absmask));
			}
			_mm_store_ps(buf, _mm_add_ps(d0, d1));
			d = buf[0] + buf[1] + buf[2] + buf[3];
#elif CV_NEON
			float32x4_t v_sum = vdupq_n_f32(0.0f);
			for (; j <= n - 4; j += 4)
				v_sum = vaddq_f32(v_sum, vabdq_f32(vld1q_f32(a + j), vld1q_f32(b + j)));

			float CV_DECL_ALIGNED(16) buf[4];
			vst1q_f32(buf, v_sum);
			d = buf[0] + buf[1] + buf[2] + buf[3];
#endif
			{
				for (; j <= n - 4; j += 4)
				{
					d += std::abs(a[j] - b[j]) + std::abs(a[j + 1] - b[j + 1]) +
						std::abs(a[j + 2] - b[j + 2]) + std::abs(a[j + 3] - b[j + 3]);
				}
			}

			for (; j < n; j++)
				d += std::abs(a[j] - b[j]);
			return d;
		}

		int normL1_(const uchar* a, const uchar* b, int n)
		{
			int j = 0, d = 0;
#if CV_SSE
			__m128i d0 = _mm_setzero_si128();

			for (; j <= n - 16; j += 16)
			{
				__m128i t0 = _mm_loadu_si128((const __m128i*)(a + j));
				__m128i t1 = _mm_loadu_si128((const __m128i*)(b + j));

				d0 = _mm_add_epi32(d0, _mm_sad_epu8(t0, t1));
			}

			for (; j <= n - 4; j += 4)
			{
				__m128i t0 = _mm_cvtsi32_si128(*(const int*)(a + j));
				__m128i t1 = _mm_cvtsi32_si128(*(const int*)(b + j));

				d0 = _mm_add_epi32(d0, _mm_sad_epu8(t0, t1));
			}
			d = _mm_cvtsi128_si32(_mm_add_epi32(d0, _mm_unpackhi_epi64(d0, d0)));
#elif CV_NEON
			uint32x4_t v_sum = vdupq_n_u32(0.0f);
			for (; j <= n - 16; j += 16)
			{
				uint8x16_t v_dst = vabdq_u8(vld1q_u8(a + j), vld1q_u8(b + j));
				uint16x8_t v_low = vmovl_u8(vget_low_u8(v_dst)), v_high = vmovl_u8(vget_high_u8(v_dst));
				v_sum = vaddq_u32(v_sum, vaddl_u16(vget_low_u16(v_low), vget_low_u16(v_high)));
				v_sum = vaddq_u32(v_sum, vaddl_u16(vget_high_u16(v_low), vget_high_u16(v_high)));
			}

			uint CV_DECL_ALIGNED(16) buf[4];
			vst1q_u32(buf, v_sum);
			d = buf[0] + buf[1] + buf[2] + buf[3];
#endif
			{
				for (; j <= n - 4; j += 4)
				{
					d += std::abs(a[j] - b[j]) + std::abs(a[j + 1] - b[j + 1]) +
						std::abs(a[j + 2] - b[j + 2]) + std::abs(a[j + 3] - b[j + 3]);
				}
			}
			for (; j < n; j++)
				d += std::abs(a[j] - b[j]);
			return d;
		}

	}
} //cv::hal

  //==================================================================================================

namespace cv
{

	template<typename T, typename ST> int
		normInf_(const T* src, const uchar* mask, ST* _result, int len, int cn)
	{
		ST result = *_result;
		if (!mask)
		{
			result = std::max(result, normInf<T, ST>(src, len*cn));
		}
		else
		{
			for (int i = 0; i < len; i++, src += cn)
				if (mask[i])
				{
					for (int k = 0; k < cn; k++)
						result = std::max(result, ST(cv_abs(src[k])));
				}
		}
		*_result = result;
		return 0;
	}

	template<typename T, typename ST> int
		normL1_(const T* src, const uchar* mask, ST* _result, int len, int cn)
	{
		ST result = *_result;
		if (!mask)
		{
			result += normL1<T, ST>(src, len*cn);
		}
		else
		{
			for (int i = 0; i < len; i++, src += cn)
				if (mask[i])
				{
					for (int k = 0; k < cn; k++)
						result += cv_abs(src[k]);
				}
		}
		*_result = result;
		return 0;
	}

	template<typename T, typename ST> int
		normL2_(const T* src, const uchar* mask, ST* _result, int len, int cn)
	{
		ST result = *_result;
		if (!mask)
		{
			result += normL2Sqr<T, ST>(src, len*cn);
		}
		else
		{
			for (int i = 0; i < len; i++, src += cn)
				if (mask[i])
				{
					for (int k = 0; k < cn; k++)
					{
						T v = src[k];
						result += (ST)v*v;
					}
				}
		}
		*_result = result;
		return 0;
	}

	template<typename T, typename ST> int
		normDiffInf_(const T* src1, const T* src2, const uchar* mask, ST* _result, int len, int cn)
	{
		ST result = *_result;
		if (!mask)
		{
			result = std::max(result, normInf<T, ST>(src1, src2, len*cn));
		}
		else
		{
			for (int i = 0; i < len; i++, src1 += cn, src2 += cn)
				if (mask[i])
				{
					for (int k = 0; k < cn; k++)
						result = std::max(result, (ST)std::abs(src1[k] - src2[k]));
				}
		}
		*_result = result;
		return 0;
	}

	template<typename T, typename ST> int
		normDiffL1_(const T* src1, const T* src2, const uchar* mask, ST* _result, int len, int cn)
	{
		ST result = *_result;
		if (!mask)
		{
			result += normL1<T, ST>(src1, src2, len*cn);
		}
		else
		{
			for (int i = 0; i < len; i++, src1 += cn, src2 += cn)
				if (mask[i])
				{
					for (int k = 0; k < cn; k++)
						result += std::abs(src1[k] - src2[k]);
				}
		}
		*_result = result;
		return 0;
	}

	template<typename T, typename ST> int
		normDiffL2_(const T* src1, const T* src2, const uchar* mask, ST* _result, int len, int cn)
	{
		ST result = *_result;
		if (!mask)
		{
			result += normL2Sqr<T, ST>(src1, src2, len*cn);
		}
		else
		{
			for (int i = 0; i < len; i++, src1 += cn, src2 += cn)
				if (mask[i])
				{
					for (int k = 0; k < cn; k++)
					{
						ST v = src1[k] - src2[k];
						result += v * v;
					}
				}
		}
		*_result = result;
		return 0;
	}

#define CV_DEF_NORM_FUNC(L, suffix, type, ntype) \
    static int norm##L##_##suffix(const type* src, const uchar* mask, ntype* r, int len, int cn) \
{ return norm##L##_(src, mask, r, len, cn); } \
    static int normDiff##L##_##suffix(const type* src1, const type* src2, \
    const uchar* mask, ntype* r, int len, int cn) \
{ return normDiff##L##_(src1, src2, mask, r, (int)len, cn); }

#define CV_DEF_NORM_ALL(suffix, type, inftype, l1type, l2type) \
    CV_DEF_NORM_FUNC(Inf, suffix, type, inftype) \
    CV_DEF_NORM_FUNC(L1, suffix, type, l1type) \
    CV_DEF_NORM_FUNC(L2, suffix, type, l2type)

	CV_DEF_NORM_ALL(8u, uchar, int, int, int)
	CV_DEF_NORM_ALL(8s, schar, int, int, int)
	CV_DEF_NORM_ALL(16u, ushort, int, int, double)
	CV_DEF_NORM_ALL(16s, short, int, int, double)
	CV_DEF_NORM_ALL(32s, int, int, double, double)
	CV_DEF_NORM_ALL(32f, float, float, double, double)
	CV_DEF_NORM_ALL(64f, double, double, double, double)


	typedef int(*NormFunc)(const uchar*, const uchar*, uchar*, int, int);
	typedef int(*NormDiffFunc)(const uchar*, const uchar*, const uchar*, uchar*, int, int);

	static NormFunc getNormFunc(int normType, int depth)
	{
		static NormFunc normTab[3][8] =
		{
		{
			(NormFunc)GET_OPTIMIZED(normInf_8u), (NormFunc)GET_OPTIMIZED(normInf_8s), (NormFunc)GET_OPTIMIZED(normInf_16u), (NormFunc)GET_OPTIMIZED(normInf_16s),
			(NormFunc)GET_OPTIMIZED(normInf_32s), (NormFunc)GET_OPTIMIZED(normInf_32f), (NormFunc)normInf_64f, 0
		},
		{
			(NormFunc)GET_OPTIMIZED(normL1_8u), (NormFunc)GET_OPTIMIZED(normL1_8s), (NormFunc)GET_OPTIMIZED(normL1_16u), (NormFunc)GET_OPTIMIZED(normL1_16s),
			(NormFunc)GET_OPTIMIZED(normL1_32s), (NormFunc)GET_OPTIMIZED(normL1_32f), (NormFunc)normL1_64f, 0
		},
		{
			(NormFunc)GET_OPTIMIZED(normL2_8u), (NormFunc)GET_OPTIMIZED(normL2_8s), (NormFunc)GET_OPTIMIZED(normL2_16u), (NormFunc)GET_OPTIMIZED(normL2_16s),
			(NormFunc)GET_OPTIMIZED(normL2_32s), (NormFunc)GET_OPTIMIZED(normL2_32f), (NormFunc)normL2_64f, 0
		}
		};

		return normTab[normType][depth];
	}

	static NormDiffFunc getNormDiffFunc(int normType, int depth)
	{
		static NormDiffFunc normDiffTab[3][8] =
		{
		{
			(NormDiffFunc)GET_OPTIMIZED(normDiffInf_8u), (NormDiffFunc)normDiffInf_8s,
			(NormDiffFunc)normDiffInf_16u, (NormDiffFunc)normDiffInf_16s,
			(NormDiffFunc)normDiffInf_32s, (NormDiffFunc)GET_OPTIMIZED(normDiffInf_32f),
			(NormDiffFunc)normDiffInf_64f, 0
		},
		{
			(NormDiffFunc)GET_OPTIMIZED(normDiffL1_8u), (NormDiffFunc)normDiffL1_8s,
			(NormDiffFunc)normDiffL1_16u, (NormDiffFunc)normDiffL1_16s,
			(NormDiffFunc)normDiffL1_32s, (NormDiffFunc)GET_OPTIMIZED(normDiffL1_32f),
			(NormDiffFunc)normDiffL1_64f, 0
		},
		{
			(NormDiffFunc)GET_OPTIMIZED(normDiffL2_8u), (NormDiffFunc)normDiffL2_8s,
			(NormDiffFunc)normDiffL2_16u, (NormDiffFunc)normDiffL2_16s,
			(NormDiffFunc)normDiffL2_32s, (NormDiffFunc)GET_OPTIMIZED(normDiffL2_32f),
			(NormDiffFunc)normDiffL2_64f, 0
		}
		};

		return normDiffTab[normType][depth];
	}

#ifdef HAVE_OPENCL

	static bool ocl_norm(InputArray _src, int normType, InputArray _mask, double & result)
	{
		const ocl::Device & d = ocl::Device::getDefault();

#ifdef __ANDROID__
		if (d.isNVidia())
			return false;
#endif
		const int cn = _src.channels();
		if (cn > 4)
			return false;
		int type = _src.type(), depth = CV_MAT_DEPTH(type);
		bool doubleSupport = d.doubleFPConfig() > 0,
			haveMask = _mask.kind() != _InputArray::NONE;

		if (!(normType == NORM_INF || normType == NORM_L1 || normType == NORM_L2 || normType == NORM_L2SQR) ||
			(!doubleSupport && depth == CV_64F))
			return false;

		UMat src = _src.getUMat();

		if (normType == NORM_INF)
		{
			if (!ocl_minMaxIdx(_src, NULL, &result, NULL, NULL, _mask,
				std::max(depth, CV_32S), depth != CV_8U && depth != CV_16U))
				return false;
		}
		else if (normType == NORM_L1 || normType == NORM_L2 || normType == NORM_L2SQR)
		{
			Scalar sc;
			bool unstype = depth == CV_8U || depth == CV_16U;

			if (!ocl_sum(haveMask ? src : src.reshape(1), sc, normType == NORM_L2 || normType == NORM_L2SQR ?
				OCL_OP_SUM_SQR : (unstype ? OCL_OP_SUM : OCL_OP_SUM_ABS), _mask))
				return false;

			double s = 0.0;
			for (int i = 0; i < (haveMask ? cn : 1); ++i)
				s += sc[i];

			result = normType == NORM_L1 || normType == NORM_L2SQR ? s : std::sqrt(s);
		}

		return true;
	}

#endif

} // cv::

double cv::norm(InputArray _src, int normType, InputArray _mask)
{
	CV_INSTRUMENT_REGION()

		normType &= NORM_TYPE_MASK;
	CV_Assert(normType == NORM_INF || normType == NORM_L1 ||
		normType == NORM_L2 || normType == NORM_L2SQR ||
		((normType == NORM_HAMMING || normType == NORM_HAMMING2) && _src.type() == CV_8U));

#if defined HAVE_OPENCL || defined HAVE_IPP
	double _result = 0;
#endif

#ifdef HAVE_OPENCL
	CV_OCL_RUN_(OCL_PERFORMANCE_CHECK(_src.isUMat()) && _src.dims() <= 2,
		ocl_norm(_src, normType, _mask, _result),
		_result)
#endif

		Mat src = _src.getMat(), mask = _mask.getMat();
	CV_IPP_RUN(IPP_VERSION_X100 >= 700, ipp_norm(src, normType, mask, _result), _result);

	int depth = src.depth(), cn = src.channels();
	if (src.isContinuous() && mask.empty())
	{
		size_t len = src.total()*cn;
		if (len == (size_t)(int)len)
		{
			if (depth == CV_32F)
			{
				const float* data = src.ptr<float>();

				if (normType == NORM_L2)
				{
					double result = 0;
					GET_OPTIMIZED(normL2_32f)(data, 0, &result, (int)len, 1);
					return std::sqrt(result);
				}
				if (normType == NORM_L2SQR)
				{
					double result = 0;
					GET_OPTIMIZED(normL2_32f)(data, 0, &result, (int)len, 1);
					return result;
				}
				if (normType == NORM_L1)
				{
					double result = 0;
					GET_OPTIMIZED(normL1_32f)(data, 0, &result, (int)len, 1);
					return result;
				}
				if (normType == NORM_INF)
				{
					float result = 0;
					GET_OPTIMIZED(normInf_32f)(data, 0, &result, (int)len, 1);
					return result;
				}
			}
			if (depth == CV_8U)
			{
				const uchar* data = src.ptr<uchar>();

				if (normType == NORM_HAMMING)
				{
					return hal::normHamming(data, (int)len);
				}

				if (normType == NORM_HAMMING2)
				{
					return hal::normHamming(data, (int)len, 2);
				}
			}
		}
	}

	CV_Assert(mask.empty() || mask.type() == CV_8U);

	if (normType == NORM_HAMMING || normType == NORM_HAMMING2)
	{
		if (!mask.empty())
		{
			Mat temp;
			bitwise_and(src, mask, temp);
			return norm(temp, normType);
		}
		int cellSize = normType == NORM_HAMMING ? 1 : 2;

		const Mat* arrays[] = { &src, 0 };
		uchar* ptrs[1];
		NAryMatIterator it(arrays, ptrs);
		int total = (int)it.size;
		int result = 0;

		for (size_t i = 0; i < it.nplanes; i++, ++it)
		{
			result += hal::normHamming(ptrs[0], total, cellSize);
		}

		return result;
	}

	NormFunc func = getNormFunc(normType >> 1, depth);
	CV_Assert(func != 0);

	const Mat* arrays[] = { &src, &mask, 0 };
	uchar* ptrs[2];
	union
	{
		double d;
		int i;
		float f;
	}
	result;
	result.d = 0;
	NAryMatIterator it(arrays, ptrs);
	int j, total = (int)it.size, blockSize = total, intSumBlockSize = 0, count = 0;
	bool blockSum = (normType == NORM_L1 && depth <= CV_16S) ||
		((normType == NORM_L2 || normType == NORM_L2SQR) && depth <= CV_8S);
	int isum = 0;
	int *ibuf = &result.i;
	size_t esz = 0;

	if (blockSum)
	{
		intSumBlockSize = (normType == NORM_L1 && depth <= CV_8S ? (1 << 23) : (1 << 15)) / cn;
		blockSize = std::min(blockSize, intSumBlockSize);
		ibuf = &isum;
		esz = src.elemSize();
	}

	for (size_t i = 0; i < it.nplanes; i++, ++it)
	{
		for (j = 0; j < total; j += blockSize)
		{
			int bsz = std::min(total - j, blockSize);
			func(ptrs[0], ptrs[1], (uchar*)ibuf, bsz, cn);
			count += bsz;
			if (blockSum && (count + blockSize >= intSumBlockSize || (i + 1 >= it.nplanes && j + bsz >= total)))
			{
				result.d += isum;
				isum = 0;
				count = 0;
			}
			ptrs[0] += bsz * esz;
			if (ptrs[1])
				ptrs[1] += bsz;
		}
	}

	if (normType == NORM_INF)
	{
		if (depth == CV_64F)
			;
		else if (depth == CV_32F)
			result.d = result.f;
		else
			result.d = result.i;
	}
	else if (normType == NORM_L2)
		result.d = std::sqrt(result.d);

	return result.d;
}

//==================================================================================================

#ifdef HAVE_OPENCL

namespace cv {

	static bool ocl_norm(InputArray _src1, InputArray _src2, int normType, InputArray _mask, double & result)
	{
#ifdef __ANDROID__
		if (ocl::Device::getDefault().isNVidia())
			return false;
#endif

		Scalar sc1, sc2;
		int cn = _src1.channels();
		if (cn > 4)
			return false;
		int type = _src1.type(), depth = CV_MAT_DEPTH(type);
		bool relative = (normType & NORM_RELATIVE) != 0;
		normType &= ~NORM_RELATIVE;
		bool normsum = normType == NORM_L1 || normType == NORM_L2 || normType == NORM_L2SQR;

#ifdef __APPLE__
		if (normType == NORM_L1 && type == CV_16UC3 && !_mask.empty())
			return false;
#endif

		if (normsum)
		{
			if (!ocl_sum(_src1, sc1, normType == NORM_L2 || normType == NORM_L2SQR ?
				OCL_OP_SUM_SQR : OCL_OP_SUM, _mask, _src2, relative, sc2))
				return false;
		}
		else
		{
			if (!ocl_minMaxIdx(_src1, NULL, &sc1[0], NULL, NULL, _mask, std::max(CV_32S, depth),
				false, _src2, relative ? &sc2[0] : NULL))
				return false;
			cn = 1;
		}

		double s2 = 0;
		for (int i = 0; i < cn; ++i)
		{
			result += sc1[i];
			if (relative)
				s2 += sc2[i];
		}

		if (normType == NORM_L2)
		{
			result = std::sqrt(result);
			if (relative)
				s2 = std::sqrt(s2);
		}

		if (relative)
			result /= (s2 + DBL_EPSILON);

		return true;
	}

}

#endif

double cv::norm(InputArray _src1, InputArray _src2, int normType, InputArray _mask)
{
	CV_INSTRUMENT_REGION()

		CV_Assert(_src1.sameSize(_src2) && _src1.type() == _src2.type());

#if defined HAVE_OPENCL || defined HAVE_IPP
	double _result = 0;
#endif

#ifdef HAVE_OPENCL
	CV_OCL_RUN_(OCL_PERFORMANCE_CHECK(_src1.isUMat()),
		ocl_norm(_src1, _src2, normType, _mask, _result),
		_result)
#endif

		CV_IPP_RUN(IPP_VERSION_X100 >= 700, ipp_norm(_src1, _src2, normType, _mask, _result), _result);

	if (normType & CV_RELATIVE)
	{
		return norm(_src1, _src2, normType & ~CV_RELATIVE, _mask) / (norm(_src2, normType, _mask) + DBL_EPSILON);
	}

	Mat src1 = _src1.getMat(), src2 = _src2.getMat(), mask = _mask.getMat();
	int depth = src1.depth(), cn = src1.channels();

	normType &= 7;
	CV_Assert(normType == NORM_INF || normType == NORM_L1 ||
		normType == NORM_L2 || normType == NORM_L2SQR ||
		((normType == NORM_HAMMING || normType == NORM_HAMMING2) && src1.type() == CV_8U));

	if (src1.isContinuous() && src2.isContinuous() && mask.empty())
	{
		size_t len = src1.total()*src1.channels();
		if (len == (size_t)(int)len)
		{
			if (src1.depth() == CV_32F)
			{
				const float* data1 = src1.ptr<float>();
				const float* data2 = src2.ptr<float>();

				if (normType == NORM_L2)
				{
					double result = 0;
					GET_OPTIMIZED(normDiffL2_32f)(data1, data2, 0, &result, (int)len, 1);
					return std::sqrt(result);
				}
				if (normType == NORM_L2SQR)
				{
					double result = 0;
					GET_OPTIMIZED(normDiffL2_32f)(data1, data2, 0, &result, (int)len, 1);
					return result;
				}
				if (normType == NORM_L1)
				{
					double result = 0;
					GET_OPTIMIZED(normDiffL1_32f)(data1, data2, 0, &result, (int)len, 1);
					return result;
				}
				if (normType == NORM_INF)
				{
					float result = 0;
					GET_OPTIMIZED(normDiffInf_32f)(data1, data2, 0, &result, (int)len, 1);
					return result;
				}
			}
		}
	}

	CV_Assert(mask.empty() || mask.type() == CV_8U);

	if (normType == NORM_HAMMING || normType == NORM_HAMMING2)
	{
		if (!mask.empty())
		{
			Mat temp;
			bitwise_xor(src1, src2, temp);
			bitwise_and(temp, mask, temp);
			return norm(temp, normType);
		}
		int cellSize = normType == NORM_HAMMING ? 1 : 2;

		const Mat* arrays[] = { &src1, &src2, 0 };
		uchar* ptrs[2];
		NAryMatIterator it(arrays, ptrs);
		int total = (int)it.size;
		int result = 0;

		for (size_t i = 0; i < it.nplanes; i++, ++it)
		{
			result += hal::normHamming(ptrs[0], ptrs[1], total, cellSize);
		}

		return result;
	}

	NormDiffFunc func = getNormDiffFunc(normType >> 1, depth);
	CV_Assert(func != 0);

	const Mat* arrays[] = { &src1, &src2, &mask, 0 };
	uchar* ptrs[3];
	union
	{
		double d;
		float f;
		int i;
		unsigned u;
	}
	result;
	result.d = 0;
	NAryMatIterator it(arrays, ptrs);
	int j, total = (int)it.size, blockSize = total, intSumBlockSize = 0, count = 0;
	bool blockSum = (normType == NORM_L1 && depth <= CV_16S) ||
		((normType == NORM_L2 || normType == NORM_L2SQR) && depth <= CV_8S);
	unsigned isum = 0;
	unsigned *ibuf = &result.u;
	size_t esz = 0;

	if (blockSum)
	{
		intSumBlockSize = normType == NORM_L1 && depth <= CV_8S ? (1 << 23) : (1 << 15);
		blockSize = std::min(blockSize, intSumBlockSize);
		ibuf = &isum;
		esz = src1.elemSize();
	}

	for (size_t i = 0; i < it.nplanes; i++, ++it)
	{
		for (j = 0; j < total; j += blockSize)
		{
			int bsz = std::min(total - j, blockSize);
			func(ptrs[0], ptrs[1], ptrs[2], (uchar*)ibuf, bsz, cn);
			count += bsz;
			if (blockSum && (count + blockSize >= intSumBlockSize || (i + 1 >= it.nplanes && j + bsz >= total)))
			{
				result.d += isum;
				isum = 0;
				count = 0;
			}
			ptrs[0] += bsz * esz;
			ptrs[1] += bsz * esz;
			if (ptrs[2])
				ptrs[2] += bsz;
		}
	}

	if (normType == NORM_INF)
	{
		if (depth == CV_64F)
			;
		else if (depth == CV_32F)
			result.d = result.f;
		else
			result.d = result.u;
	}
	else if (normType == NORM_L2)
		result.d = std::sqrt(result.d);

	return result.d;
}

cv::Hamming::ResultType cv::Hamming::operator()(const unsigned char* a, const unsigned char* b, int size) const
{
	return cv::hal::normHamming(a, b, size);
}

double cv::PSNR(InputArray _src1, InputArray _src2)
{
	CV_INSTRUMENT_REGION()

		//Input arrays must have depth CV_8U
	CV_Assert(_src1.depth() == CV_8U && _src2.depth() == CV_8U);

	double diff = std::sqrt(norm(_src1, _src2, NORM_L2SQR) / (_src1.total()*_src1.channels()));
	return 20 * log10(255. / (diff + DBL_EPSILON));
}
