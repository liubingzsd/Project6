#include "precomp.hpp"
#include "../include/opencv2/core/utils/configuration.private.hpp"

////////////////////////////////////////// kmeans ////////////////////////////////////////////

namespace cv
{

	static int CV_KMEANS_PARALLEL_GRANULARITY = 1000;// (int)utils::getConfigurationParameterSizeT("OPENCV_KMEANS_PARALLEL_GRANULARITY", 1000);

	static void generateRandomCenter(int dims, const Vec2f* box, float* center, RNG& rng)
	{
		float margin = 1.f / dims;
		for (int j = 0; j < dims; j++)
			center[j] = ((float)rng*(1.f + margin * 2.f) - margin)*(box[j][1] - box[j][0]) + box[j][0];
	}

	class KMeansPPDistanceComputer : public ParallelLoopBody
	{
	public:
		KMeansPPDistanceComputer(float *tdist2_, const Mat& data_, const float *dist_, int ci_) :
			tdist2(tdist2_), data(data_), dist(dist_), ci(ci_)
		{ }

		void operator()(const cv::Range& range) const
		{
			CV_TRACE_FUNCTION();
			const int begin = range.start;
			const int end = range.end;
			const int dims = data.cols;

			for (int i = begin; i<end; i++)
			{
				tdist2[i] = std::min(normL2Sqr(data.ptr<float>(i), data.ptr<float>(ci), dims), dist[i]);
			}
		}

	private:
		KMeansPPDistanceComputer & operator=(const KMeansPPDistanceComputer&); // = delete

		float *tdist2;
		const Mat& data;
		const float *dist;
		const int ci;
	};

	/*
	k-means center initialization using the following algorithm:
	Arthur & Vassilvitskii (2007) k-means++: The Advantages of Careful Seeding
	*/
	static void generateCentersPP(const Mat& data, Mat& _out_centers,
		int K, RNG& rng, int trials)
	{
		CV_TRACE_FUNCTION();
		const int dims = data.cols, N = data.rows;
		cv::AutoBuffer<int, 64> _centers(K);
		int* centers = &_centers[0];
		cv::AutoBuffer<float, 0> _dist(N * 3);
		float* dist = &_dist[0], *tdist = dist + N, *tdist2 = tdist + N;
		double sum0 = 0;

		centers[0] = (unsigned)rng % N;

		for (int i = 0; i < N; i++)
		{
			dist[i] = normL2Sqr(data.ptr<float>(i), data.ptr<float>(centers[0]), dims);
			sum0 += dist[i];
		}

		for (int k = 1; k < K; k++)
		{
			double bestSum = DBL_MAX;
			int bestCenter = -1;

			for (int j = 0; j < trials; j++)
			{
				double p = (double)rng*sum0;
				int ci = 0;
				for (; ci < N - 1; ci++)
				{
					p -= dist[ci];
					if (p <= 0)
						break;
				}

				parallel_for_(Range(0, N),
					KMeansPPDistanceComputer(tdist2, data, dist, ci),
					divUp(dims * N, CV_KMEANS_PARALLEL_GRANULARITY));
				double s = 0;
				for (int i = 0; i < N; i++)
				{
					s += tdist2[i];
				}

				if (s < bestSum)
				{
					bestSum = s;
					bestCenter = ci;
					std::swap(tdist, tdist2);
				}
			}
			centers[k] = bestCenter;
			sum0 = bestSum;
			std::swap(dist, tdist);
		}

		for (int k = 0; k < K; k++)
		{
			const float* src = data.ptr<float>(centers[k]);
			float* dst = _out_centers.ptr<float>(k);
			for (int j = 0; j < dims; j++)
				dst[j] = src[j];
		}
	}

	template<bool onlyDistance>
	class KMeansDistanceComputer : public ParallelLoopBody
	{
	public:
		KMeansDistanceComputer(double *distances_,
			int *labels_,
			const Mat& data_,
			const Mat& centers_)
			: distances(distances_),
			labels(labels_),
			data(data_),
			centers(centers_)
		{
		}

		void operator()(const Range& range) const
		{
			CV_TRACE_FUNCTION();
			const int begin = range.start;
			const int end = range.end;
			const int K = centers.rows;
			const int dims = centers.cols;

			for (int i = begin; i < end; ++i)
			{
				const float *sample = data.ptr<float>(i);
				if (onlyDistance)
				{
					const float* center = centers.ptr<float>(labels[i]);
					distances[i] = normL2Sqr(sample, center, dims);
					continue;
				}
				else
				{
					int k_best = 0;
					double min_dist = DBL_MAX;

					for (int k = 0; k < K; k++)
					{
						const float* center = centers.ptr<float>(k);
						const double dist = normL2Sqr(sample, center, dims);

						if (min_dist > dist)
						{
							min_dist = dist;
							k_best = k;
						}
					}

					distances[i] = min_dist;
					labels[i] = k_best;
				}
			}
		}

	private:
		KMeansDistanceComputer & operator=(const KMeansDistanceComputer&); // = delete

		double *distances;
		int *labels;
		const Mat& data;
		const Mat& centers;
	};

}

double cv::kmeans(InputArray _data, int K,
	InputOutputArray _bestLabels,
	TermCriteria criteria, int attempts,
	int flags, OutputArray _centers)
{
	CV_INSTRUMENT_REGION()
	const int SPP_TRIALS = 3;
	Mat data0 = _data.getMat();
	const bool isrow = data0.rows == 1;
	const int N = isrow ? data0.cols : data0.rows;
	const int dims = (isrow ? 1 : data0.cols)*data0.channels();
	const int type = data0.depth();

	attempts = std::max(attempts, 1);
	CV_Assert(data0.dims <= 2 && type == CV_32F && K > 0);
	CV_Assert(N >= K);

	Mat data(N, dims, CV_32F, data0.ptr(), isrow ? dims * sizeof(float) : static_cast<size_t>(data0.step));

	_bestLabels.create(N, 1, CV_32S, -1, true);

	Mat _labels, best_labels = _bestLabels.getMat();
	if (flags & CV_KMEANS_USE_INITIAL_LABELS)
	{
		CV_Assert((best_labels.cols == 1 || best_labels.rows == 1) &&
			best_labels.cols*best_labels.rows == N &&
			best_labels.type() == CV_32S &&
			best_labels.isContinuous());
		best_labels.reshape(1, N).copyTo(_labels);
		for (int i = 0; i < N; i++)
		{
			CV_Assert((unsigned)_labels.at<int>(i) < (unsigned)K);
		}
	}
	else
	{
		if (!((best_labels.cols == 1 || best_labels.rows == 1) &&
			best_labels.cols*best_labels.rows == N &&
			best_labels.type() == CV_32S &&
			best_labels.isContinuous()))
		{
			_bestLabels.create(N, 1, CV_32S);
			best_labels = _bestLabels.getMat();
		}
		_labels.create(best_labels.size(), best_labels.type());
	}
	int* labels = _labels.ptr<int>();

	Mat centers(K, dims, type), old_centers(K, dims, type), temp(1, dims, type);
	cv::AutoBuffer<int, 64> counters(K);
	cv::AutoBuffer<double, 64> dists(N);
	RNG& rng = theRNG();

	if (criteria.type & TermCriteria::EPS)
		criteria.epsilon = std::max(criteria.epsilon, 0.);
	else
		criteria.epsilon = FLT_EPSILON;
	criteria.epsilon *= criteria.epsilon;

	if (criteria.type & TermCriteria::COUNT)
		criteria.maxCount = std::min(std::max(criteria.maxCount, 2), 100);
	else
		criteria.maxCount = 100;

	if (K == 1)
	{
		attempts = 1;
		criteria.maxCount = 2;
	}

	cv::AutoBuffer<Vec2f, 64> box(dims);
	if (!(flags & KMEANS_PP_CENTERS))
	{
		{
			const float* sample = data.ptr<float>(0);
			for (int j = 0; j < dims; j++)
				box[j] = Vec2f(sample[j], sample[j]);
		}
		for (int i = 1; i < N; i++)
		{
			const float* sample = data.ptr<float>(i);
			for (int j = 0; j < dims; j++)
			{
				float v = sample[j];
				box[j][0] = std::min(box[j][0], v);
				box[j][1] = std::max(box[j][1], v);
			}
		}
	}

	double best_compactness = DBL_MAX;
	for (int a = 0; a < attempts; a++)
	{
		double compactness = 0;

		for (int iter = 0; ;)
		{
			double max_center_shift = iter == 0 ? DBL_MAX : 0.0;

			swap(centers, old_centers);

			if (iter == 0 && (a > 0 || !(flags & KMEANS_USE_INITIAL_LABELS)))
			{
				if (flags & KMEANS_PP_CENTERS)
					generateCentersPP(data, centers, K, rng, SPP_TRIALS);
				else
				{
					for (int k = 0; k < K; k++)
						generateRandomCenter(dims, box, centers.ptr<float>(k), rng);
				}
			}
			else
			{
				// compute centers
				centers = Scalar(0);
				for (int k = 0; k < K; k++)
					counters[k] = 0;

				for (int i = 0; i < N; i++)
				{
					const float* sample = data.ptr<float>(i);
					int k = labels[i];
					float* center = centers.ptr<float>(k);
					for (int j = 0; j < dims; j++)
						center[j] += sample[j];
					counters[k]++;
				}

				for (int k = 0; k < K; k++)
				{
					if (counters[k] != 0)
						continue;

					// if some cluster appeared to be empty then:
					//   1. find the biggest cluster
					//   2. find the farthest from the center point in the biggest cluster
					//   3. exclude the farthest point from the biggest cluster and form a new 1-point cluster.
					int max_k = 0;
					for (int k1 = 1; k1 < K; k1++)
					{
						if (counters[max_k] < counters[k1])
							max_k = k1;
					}

					double max_dist = 0;
					int farthest_i = -1;
					float* base_center = centers.ptr<float>(max_k);
					float* _base_center = temp.ptr<float>(); // normalized
					float scale = 1.f / counters[max_k];
					for (int j = 0; j < dims; j++)
						_base_center[j] = base_center[j] * scale;

					for (int i = 0; i < N; i++)
					{
						if (labels[i] != max_k)
							continue;
						const float* sample = data.ptr<float>(i);
						double dist = normL2Sqr(sample, _base_center, dims);

						if (max_dist <= dist)
						{
							max_dist = dist;
							farthest_i = i;
						}
					}

					counters[max_k]--;
					counters[k]++;
					labels[farthest_i] = k;

					const float* sample = data.ptr<float>(farthest_i);
					float* cur_center = centers.ptr<float>(k);
					for (int j = 0; j < dims; j++)
					{
						base_center[j] -= sample[j];
						cur_center[j] += sample[j];
					}
				}

				for (int k = 0; k < K; k++)
				{
					float* center = centers.ptr<float>(k);
					CV_Assert(counters[k] != 0);

					float scale = 1.f / counters[k];
					for (int j = 0; j < dims; j++)
						center[j] *= scale;

					if (iter > 0)
					{
						double dist = 0;
						const float* old_center = old_centers.ptr<float>(k);
						for (int j = 0; j < dims; j++)
						{
							double t = center[j] - old_center[j];
							dist += t * t;
						}
						max_center_shift = std::max(max_center_shift, dist);
					}
				}
			}

			bool isLastIter = (++iter == MAX(criteria.maxCount, 2) || max_center_shift <= criteria.epsilon);

			if (isLastIter)
			{
				// don't re-assign labels to avoid creation of empty clusters
				parallel_for_(Range(0, N), KMeansDistanceComputer<true>(dists, labels, data, centers), divUp(dims * N, CV_KMEANS_PARALLEL_GRANULARITY));
				compactness = sum(Mat(Size(N, 1), CV_64F, &dists[0]))[0];
				break;
			}
			else
			{
				// assign labels
				parallel_for_(Range(0, N), KMeansDistanceComputer<false>(dists, labels, data, centers), divUp(dims * N * K, CV_KMEANS_PARALLEL_GRANULARITY));
			}
		}

		if (compactness < best_compactness)
		{
			best_compactness = compactness;
			if (_centers.needed())
			{
				if (_centers.fixedType() && _centers.channels() == dims)
					centers.reshape(dims).copyTo(_centers);
				else
					centers.copyTo(_centers);
			}
			_labels.copyTo(best_labels);
		}
	}

	return best_compactness;
}
