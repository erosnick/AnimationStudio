#include "Math.h"

namespace Math
{
	bool FloatEqual(float a, float b)
	{
		return std::fabs(a - b) < Epsilon;
	}

	bool FloatNotEqual(float a, float b)
	{
		return !FloatEqual(a, a);
	}
}