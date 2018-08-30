#include "../include/opencv2/core/hal/intrin.hpp"
#define CV_CPU_OPTIMIZATION_DECLARATIONS_ONLY
namespace cv {
	namespace hal {

		extern const uchar popCountTable[256];
			// forward declarations
		int normHamming(const uchar* a, int n);
		int normHamming(const uchar* a, const uchar* b, int n);

#ifndef CV_CPU_OPTIMIZATION_DECLARATIONS_ONLY

		int normHamming(const uchar* a, int n)
		{

			int i = 0;
			int result = 0;
#if CV_POPCNT
			{
#  if defined CV_POPCNT_U64
				for (; i <= n - 8; i += 8)
				{
					result += (int)CV_POPCNT_U64(*(uint64*)(a + i));
				}
#  endif
				for (; i <= n - 4; i += 4)
				{
					result += CV_POPCNT_U32(*(uint*)(a + i));
				}
			}
#endif // CV_POPCNT

#if CV_SIMD128
			{
				v_uint32x4 t = v_setzero_u32();
				for (; i <= n - v_uint8x16::nlanes; i += v_uint8x16::nlanes)
				{
					t += v_popcount(v_load(a + i));
				}
				result += v_reduce_sum(t);
			}
#endif // CV_SIMD128
#if CV_ENABLE_UNROLLED
			for (; i <= n - 4; i += 4)
			{
				result += popCountTable[a[i]] + popCountTable[a[i + 1]] +
					popCountTable[a[i + 2]] + popCountTable[a[i + 3]];
			}
#endif
			for (; i < n; i++)
			{
				result += popCountTable[a[i]];
			}
			return result;
		}

		int normHamming(const uchar* a, const uchar* b, int n)
		{
			int i = 0;
			int result = 0;
#if CV_POPCNT
			{
#  if defined CV_POPCNT_U64
				for (; i <= n - 8; i += 8)
				{
					result += (int)CV_POPCNT_U64(*(uint64*)(a + i) ^ *(uint64*)(b + i));
				}
#  endif
				for (; i <= n - 4; i += 4)
				{
					result += CV_POPCNT_U32(*(uint*)(a + i) ^ *(uint*)(b + i));
				}
			}
#endif // CV_POPCNT

#if CV_SIMD128
			{
				v_uint32x4 t = v_setzero_u32();
				for (; i <= n - v_uint8x16::nlanes; i += v_uint8x16::nlanes)
				{
					t += v_popcount(v_load(a + i) ^ v_load(b + i));
				}
				result += v_reduce_sum(t);
			}
#endif // CV_SIMD128
#if CV_ENABLE_UNROLLED
			for (; i <= n - 4; i += 4)
			{
				result += popCountTable[a[i] ^ b[i]] + popCountTable[a[i + 1] ^ b[i + 1]] +
					popCountTable[a[i + 2] ^ b[i + 2]] + popCountTable[a[i + 3] ^ b[i + 3]];
			}
#endif
			for (; i < n; i++)
			{
				result += popCountTable[a[i] ^ b[i]];
			}
			return result;
		}

#endif // CV_CPU_OPTIMIZATION_DECLARATIONS_ONLY
	}
} //cv::h