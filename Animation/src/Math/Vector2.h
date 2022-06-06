#pragma once

namespace Math
{
	template<typename T>
	struct TVector2
	{
		union
		{
			struct
			{
				T x;
				T y;
			};
			T elements[2];
		};

		inline TVector2() : x(T(0)), y(T(0)) {}
		inline TVector2(T inX, T inY) : x(inX), y(inY) {}
		inline TVector2(T* data) : x(data[0]), y(data[1]) {}
	};

	typedef TVector2<float> Vector2;
	typedef TVector2<int> Vector2i;
}
