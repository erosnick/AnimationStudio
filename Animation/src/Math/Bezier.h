#pragma once

namespace Math
{
	// Draw the actual curve
	// Resolution is 200 steps since last point is i + 1
	//for (int i = 0; i < 199; ++i) {
	//	float t0 = (float)i / 199.0f;
	//	float t1 = (float)(i + 1) / 199.0f;
	//	Vector3 thisPoint = Evaluate(t0);
	//	Vector3 nextPoint = Evaluate(t1);
	//	DrawLine(thisPoint, nextPoint, magenta);
	//}
	template <typename T>
	class Bezier
	{
	public:
		Bezier()
		{
		}

		Bezier(const T& p0, const T& p1, const T& p2, const T& p3)
		{
			this->p0 = p0;
			this->p1 = p1;
			this->p2 = p2;
			this->p3 = p3;
		}

		T Evaluate(float t) const
		{
			return (1 - t) * (1 - t) * (1 - t) * p0 +
				3 * (1 - t) * (1 - t) * t * p1 +
				3 * (1 - t) * t * t * p2 +
				t * t * t * p3;
		}

		T p0;
		T p1;
		T p2;
		T p3;
	};

	template<typename T>
	T Hermite(float t)
	{
		return
			p1 * ((1.0f + 2.0f * t) * ((1.0f - t) * (1.0f - t))) +
			s1 * (t * ((1.0f - t) * (1.0f - t))) +
			p2 * ((t * t) * (3.0f - 2.0f * t)) +
			s2 * ((t * t) * (t - 1.0f));
	}
}