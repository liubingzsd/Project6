#include "precomp.hpp"
#include <limits>

namespace cv
{
	const float B2YF = 0.114f;
	const float G2YF = 0.587f;
	const float R2YF = 0.299f;

	enum
	{
		yuv_shift = 14,
		xyz_shift = 12,
		R2Y = 4899, // == R2YF*16384
		G2Y = 9617, // == G2YF*16384
		B2Y = 1868, // == B2YF*16384
		BLOCK_SIZE = 256
	};


	template<typename _Tp> struct RGB2Gray
	{
		typedef _Tp channel_type;

		RGB2Gray(int _srccn, int blueIdx, const float* _coeffs) : srccn(_srccn)
		{
			static const float coeffs0[] = { R2YF, G2YF, B2YF };
			memcpy(coeffs, _coeffs ? _coeffs : coeffs0, 3 * sizeof(coeffs[0]));
			if (blueIdx == 0)
				std::swap(coeffs[0], coeffs[2]);
		}

		void operator()(const _Tp* src, _Tp* dst, int n) const
		{
			int scn = srccn;
			float cb = coeffs[0], cg = coeffs[1], cr = coeffs[2];
			for (int i = 0; i < n; i++, src += scn)
				dst[i] = saturate_cast<_Tp>(src[0] * cb + src[1] * cg + src[2] * cr);
		}
		int srccn;
		float coeffs[3];
	};

	template<> struct RGB2Gray<uchar>
	{
		typedef uchar channel_type;

		RGB2Gray(int _srccn, int blueIdx, const int* coeffs) : srccn(_srccn)
		{
			const int coeffs0[] = { R2Y, G2Y, B2Y };
			if (!coeffs) coeffs = coeffs0;

			int b = 0, g = 0, r = (1 << (yuv_shift - 1));
			int db = coeffs[blueIdx ^ 2], dg = coeffs[1], dr = coeffs[blueIdx];

			for (int i = 0; i < 256; i++, b += db, g += dg, r += dr)
			{
				tab[i] = b;
				tab[i + 256] = g;
				tab[i + 512] = r;
			}
		}
		void operator()(const uchar* src, uchar* dst, int n) const
		{
			int scn = srccn;
			const int* _tab = tab;
			for (int i = 0; i < n; i++, src += scn)
				dst[i] = (uchar)((_tab[src[0]] + _tab[src[1] + 256] + _tab[src[2] + 512]) >> yuv_shift);
		}
		int srccn;
		int tab[256 * 3];
	};

#if CV_NEON

	template <>
	struct RGB2Gray<ushort>
	{
		typedef ushort channel_type;

		RGB2Gray(int _srccn, int blueIdx, const int* _coeffs) :
			srccn(_srccn)
		{
			static const int coeffs0[] = { R2Y, G2Y, B2Y };
			memcpy(coeffs, _coeffs ? _coeffs : coeffs0, 3 * sizeof(coeffs[0]));
			if (blueIdx == 0)
				std::swap(coeffs[0], coeffs[2]);

			v_cb = vdup_n_u16(coeffs[0]);
			v_cg = vdup_n_u16(coeffs[1]);
			v_cr = vdup_n_u16(coeffs[2]);
			v_delta = vdupq_n_u32(1 << (yuv_shift - 1));
		}

		void operator()(const ushort* src, ushort* dst, int n) const
		{
			int scn = srccn, cb = coeffs[0], cg = coeffs[1], cr = coeffs[2], i = 0;

			for (; i <= n - 8; i += 8, src += scn * 8)
			{
				uint16x8_t v_b, v_r, v_g;
				if (scn == 3)
				{
					uint16x8x3_t v_src = vld3q_u16(src);
					v_b = v_src.val[0];
					v_g = v_src.val[1];
					v_r = v_src.val[2];
				}
				else
				{
					uint16x8x4_t v_src = vld4q_u16(src);
					v_b = v_src.val[0];
					v_g = v_src.val[1];
					v_r = v_src.val[2];
				}

				uint32x4_t v_dst0_ = vmlal_u16(vmlal_u16(
					vmull_u16(vget_low_u16(v_b), v_cb),
					vget_low_u16(v_g), v_cg),
					vget_low_u16(v_r), v_cr);
				uint32x4_t v_dst1_ = vmlal_u16(vmlal_u16(
					vmull_u16(vget_high_u16(v_b), v_cb),
					vget_high_u16(v_g), v_cg),
					vget_high_u16(v_r), v_cr);

				uint16x4_t v_dst0 = vmovn_u32(vshrq_n_u32(vaddq_u32(v_dst0_, v_delta), yuv_shift));
				uint16x4_t v_dst1 = vmovn_u32(vshrq_n_u32(vaddq_u32(v_dst1_, v_delta), yuv_shift));

				vst1q_u16(dst + i, vcombine_u16(v_dst0, v_dst1));
			}

			for (; i <= n - 4; i += 4, src += scn * 4)
			{
				uint16x4_t v_b, v_r, v_g;
				if (scn == 3)
				{
					uint16x4x3_t v_src = vld3_u16(src);
					v_b = v_src.val[0];
					v_g = v_src.val[1];
					v_r = v_src.val[2];
				}
				else
				{
					uint16x4x4_t v_src = vld4_u16(src);
					v_b = v_src.val[0];
					v_g = v_src.val[1];
					v_r = v_src.val[2];
				}

				uint32x4_t v_dst = vmlal_u16(vmlal_u16(
					vmull_u16(v_b, v_cb),
					v_g, v_cg),
					v_r, v_cr);

				vst1_u16(dst + i, vmovn_u32(vshrq_n_u32(vaddq_u32(v_dst, v_delta), yuv_shift)));
			}

			for (; i < n; i++, src += scn)
				dst[i] = (ushort)CV_DESCALE((unsigned)(src[0] * cb + src[1] * cg + src[2] * cr), yuv_shift);
		}

		int srccn, coeffs[3];
		uint16x4_t v_cb, v_cg, v_cr;
		uint32x4_t v_delta;
	};

	template <>
	struct RGB2Gray<float>
	{
		typedef float channel_type;

		RGB2Gray(int _srccn, int blueIdx, const float* _coeffs) : srccn(_srccn)
		{
			static const float coeffs0[] = { R2YF, G2YF, B2YF };
			memcpy(coeffs, _coeffs ? _coeffs : coeffs0, 3 * sizeof(coeffs[0]));
			if (blueIdx == 0)
				std::swap(coeffs[0], coeffs[2]);

			v_cb = vdupq_n_f32(coeffs[0]);
			v_cg = vdupq_n_f32(coeffs[1]);
			v_cr = vdupq_n_f32(coeffs[2]);
		}

		void operator()(const float * src, float * dst, int n) const
		{
			int scn = srccn, i = 0;
			float cb = coeffs[0], cg = coeffs[1], cr = coeffs[2];

			if (scn == 3)
			{
				for (; i <= n - 8; i += 8, src += scn * 8)
				{
					float32x4x3_t v_src = vld3q_f32(src);
					vst1q_f32(dst + i, vmlaq_f32(vmlaq_f32(vmulq_f32(v_src.val[0], v_cb), v_src.val[1], v_cg), v_src.val[2], v_cr));

					v_src = vld3q_f32(src + scn * 4);
					vst1q_f32(dst + i + 4, vmlaq_f32(vmlaq_f32(vmulq_f32(v_src.val[0], v_cb), v_src.val[1], v_cg), v_src.val[2], v_cr));
				}

				for (; i <= n - 4; i += 4, src += scn * 4)
				{
					float32x4x3_t v_src = vld3q_f32(src);
					vst1q_f32(dst + i, vmlaq_f32(vmlaq_f32(vmulq_f32(v_src.val[0], v_cb), v_src.val[1], v_cg), v_src.val[2], v_cr));
				}
			}
			else
			{
				for (; i <= n - 8; i += 8, src += scn * 8)
				{
					float32x4x4_t v_src = vld4q_f32(src);
					vst1q_f32(dst + i, vmlaq_f32(vmlaq_f32(vmulq_f32(v_src.val[0], v_cb), v_src.val[1], v_cg), v_src.val[2], v_cr));

					v_src = vld4q_f32(src + scn * 4);
					vst1q_f32(dst + i + 4, vmlaq_f32(vmlaq_f32(vmulq_f32(v_src.val[0], v_cb), v_src.val[1], v_cg), v_src.val[2], v_cr));
				}

				for (; i <= n - 4; i += 4, src += scn * 4)
				{
					float32x4x4_t v_src = vld4q_f32(src);
					vst1q_f32(dst + i, vmlaq_f32(vmlaq_f32(vmulq_f32(v_src.val[0], v_cb), v_src.val[1], v_cg), v_src.val[2], v_cr));
				}
			}

			for (; i < n; i++, src += scn)
				dst[i] = src[0] * cb + src[1] * cg + src[2] * cr;
		}

		int srccn;
		float coeffs[3];
		float32x4_t v_cb, v_cg, v_cr;
	};
#endif

	template <typename Cvt>
	class CvtColorLoop_Invoker : public ParallelLoopBody
	{
		typedef typename Cvt::channel_type _Tp;
	public:

		CvtColorLoop_Invoker(const uchar * src_data_, size_t src_step_, uchar * dst_data_, size_t dst_step_, int width_, const Cvt& _cvt) :
			ParallelLoopBody(), src_data(src_data_), src_step(src_step_), dst_data(dst_data_), dst_step(dst_step_),
			width(width_), cvt(_cvt)
		{
		}

		virtual void operator()(const Range& range) const
		{
			const uchar* yS = src_data + static_cast<size_t>(range.start) * src_step;
			uchar* yD = dst_data + static_cast<size_t>(range.start) * dst_step;

			for (int i = range.start; i < range.end; ++i, yS += src_step, yD += dst_step)
				cvt(reinterpret_cast<const _Tp*>(yS), reinterpret_cast<_Tp*>(yD), width);
		}

	private:
		const uchar * src_data;
		size_t src_step;
		uchar * dst_data;
		size_t dst_step;
		int width;
		const Cvt& cvt;
		const CvtColorLoop_Invoker& operator= (const CvtColorLoop_Invoker&);
	};


	template <typename Cvt>
	void CvtColorLoop(const uchar * src_data, size_t src_step, uchar * dst_data, size_t dst_step, int width, int height, const Cvt& cvt)
	{
		parallel_for_(Range(0, height),
			CvtColorLoop_Invoker<Cvt>(src_data, src_step, dst_data, dst_step, width, cvt),
			(width * height) / static_cast<double>(1 << 16));
	}
}
namespace cv
{
	namespace hal
	{
		void cvtBGRtoGray(const uchar * src_data, size_t src_step,
			uchar * dst_data, size_t dst_step,
			int width, int height,
			int depth, int scn, bool swapBlue)
		{

			int blueIdx = swapBlue ? 2 : 0;
			if (depth == CV_8U)
				CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, RGB2Gray<uchar>(scn, blueIdx, 0));
			else if (depth == CV_16U)
				CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, RGB2Gray<ushort>(scn, blueIdx, 0));
			else
				CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, RGB2Gray<float>(scn, blueIdx, 0));
		}
	}
}

void cv::cvtColor(InputArray _src, OutputArray _dst, int code, int dcn)
{
	int stype = _src.type();
	int scn = CV_MAT_CN(stype), depth = CV_MAT_DEPTH(stype);

	Mat src, dst;
	if (_src.getObj() == _dst.getObj()) // inplace processing (#6653)
		_src.copyTo(src);
	else
		src = _src.getMat();
	Size sz = src.size();

	_dst.create(sz, CV_MAKETYPE(depth, 1));
	dst = _dst.getMat();
	hal::cvtBGRtoGray(src.data, src.step, dst.data, dst.step, src.cols, src.rows,
		depth, scn, false);

}
