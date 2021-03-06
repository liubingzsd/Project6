#include "precomp.hpp"

namespace cv
{

	////////////////////// KeyPoint //////////////////////

	size_t KeyPoint::hash() const
	{
		size_t _Val = 2166136261U, scale = 16777619U;
		Cv32suf u;
		u.f = pt.x; _Val = (scale * _Val) ^ u.u;
		u.f = pt.y; _Val = (scale * _Val) ^ u.u;
		u.f = size; _Val = (scale * _Val) ^ u.u;
		u.f = angle; _Val = (scale * _Val) ^ u.u;
		u.f = response; _Val = (scale * _Val) ^ u.u;
		_Val = (scale * _Val) ^ ((size_t)octave);
		_Val = (scale * _Val) ^ ((size_t)class_id);
		return _Val;
	}

	void KeyPoint::convert(const std::vector<KeyPoint>& keypoints, std::vector<Point2f>& points2f,
		const std::vector<int>& keypointIndexes)
	{
		CV_INSTRUMENT_REGION()

			if (keypointIndexes.empty())
			{
				points2f.resize(keypoints.size());
				for (size_t i = 0; i < keypoints.size(); i++)
					points2f[i] = keypoints[i].pt;
			}
			else
			{
				points2f.resize(keypointIndexes.size());
				for (size_t i = 0; i < keypointIndexes.size(); i++)
				{
					int idx = keypointIndexes[i];
					if (idx >= 0)
						points2f[i] = keypoints[idx].pt;
					else
					{
						CV_Error(CV_StsBadArg, "keypointIndexes has element < 0. TODO: process this case");
						//points2f[i] = Point2f(-1, -1);
					}
				}
			}
	}

	void KeyPoint::convert(const std::vector<Point2f>& points2f, std::vector<KeyPoint>& keypoints,
		float size, float response, int octave, int class_id)
	{
		CV_INSTRUMENT_REGION()

			keypoints.resize(points2f.size());
		for (size_t i = 0; i < points2f.size(); i++)
			keypoints[i] = KeyPoint(points2f[i], size, -1, response, octave, class_id);
	}

	float KeyPoint::overlap(const KeyPoint& kp1, const KeyPoint& kp2)
	{
		float a = kp1.size * 0.5f;
		float b = kp2.size * 0.5f;
		float a_2 = a * a;
		float b_2 = b * b;

		Point2f p1 = kp1.pt;
		Point2f p2 = kp2.pt;
		float c = (float)norm(p1 - p2);

		float ovrl = 0.f;

		// one circle is completely encovered by the other => no intersection points!
		if (std::min(a, b) + c <= std::max(a, b))
			return std::min(a_2, b_2) / std::max(a_2, b_2);

		if (c < a + b) // circles intersect
		{
			float c_2 = c * c;
			float cosAlpha = (b_2 + c_2 - a_2) / (kp2.size * c);
			float cosBeta = (a_2 + c_2 - b_2) / (kp1.size * c);
			float alpha = acos(cosAlpha);
			float beta = acos(cosBeta);
			float sinAlpha = sin(alpha);
			float sinBeta = sin(beta);

			float segmentAreaA = a_2 * beta;
			float segmentAreaB = b_2 * alpha;

			float triangleAreaA = a_2 * sinBeta * cosBeta;
			float triangleAreaB = b_2 * sinAlpha * cosAlpha;

			float intersectionArea = segmentAreaA + segmentAreaB - triangleAreaA - triangleAreaB;
			float unionArea = (a_2 + b_2) * (float)CV_PI - intersectionArea;

			ovrl = intersectionArea / unionArea;
		}

		return ovrl;
	}

	////////////////////// RotatedRect //////////////////////

	RotatedRect::RotatedRect(const Point2f& _point1, const Point2f& _point2, const Point2f& _point3)
	{
		Point2f _center = 0.5f * (_point1 + _point3);
		Vec2f vecs[2];
		vecs[0] = Vec2f(_point1 - _point2);
		vecs[1] = Vec2f(_point2 - _point3);
		// check that given sides are perpendicular
		CV_Assert(abs(vecs[0].dot(vecs[1])) / (norm(vecs[0]) * norm(vecs[1])) <= FLT_EPSILON);

		// wd_i stores which vector (0,1) or (1,2) will make the width
		// One of them will definitely have slope within -1 to 1
		int wd_i = 0;
		if (abs(vecs[1][1]) < abs(vecs[1][0])) wd_i = 1;
		int ht_i = (wd_i + 1) % 2;

		float _angle = atan(vecs[wd_i][1] / vecs[wd_i][0]) * 180.0f / (float)CV_PI;
		float _width = (float)norm(vecs[wd_i]);
		float _height = (float)norm(vecs[ht_i]);

		center = _center;
		size = Size2f(_width, _height);
		angle = _angle;
	}

	void RotatedRect::points(Point2f pt[]) const
	{
		double _angle = angle * CV_PI / 180.;
		float b = (float)cos(_angle)*0.5f;
		float a = (float)sin(_angle)*0.5f;

		pt[0].x = center.x - a * size.height - b * size.width;
		pt[0].y = center.y + b * size.height - a * size.width;
		pt[1].x = center.x + a * size.height - b * size.width;
		pt[1].y = center.y - b * size.height - a * size.width;
		pt[2].x = 2 * center.x - pt[0].x;
		pt[2].y = 2 * center.y - pt[0].y;
		pt[3].x = 2 * center.x - pt[1].x;
		pt[3].y = 2 * center.y - pt[1].y;
	}

	Rect RotatedRect::boundingRect() const
	{
		Point2f pt[4];
		points(pt);
		Rect r(cvFloor(std::min(std::min(std::min(pt[0].x, pt[1].x), pt[2].x), pt[3].x)),
			cvFloor(std::min(std::min(std::min(pt[0].y, pt[1].y), pt[2].y), pt[3].y)),
			cvCeil(std::max(std::max(std::max(pt[0].x, pt[1].x), pt[2].x), pt[3].x)),
			cvCeil(std::max(std::max(std::max(pt[0].y, pt[1].y), pt[2].y), pt[3].y)));
		r.width -= r.x - 1;
		r.height -= r.y - 1;
		return r;
	}


	Rect_<float> RotatedRect::boundingRect2f() const
	{
		Point2f pt[4];
		points(pt);
		Rect_<float> r(Point_<float>(min(min(min(pt[0].x, pt[1].x), pt[2].x), pt[3].x), min(min(min(pt[0].y, pt[1].y), pt[2].y), pt[3].y)),
			Point_<float>(max(max(max(pt[0].x, pt[1].x), pt[2].x), pt[3].x), max(max(max(pt[0].y, pt[1].y), pt[2].y), pt[3].y)));
		return r;
	}

} // cv
