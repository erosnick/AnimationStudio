#pragma once

namespace Math
{
	template<typename T>
	struct TVector4
	{
		union
		{
			struct 
			{
				T x;
				T y;
				T z;
				T w;
			};
			T elements[4];
		};
		inline TVector4<T>() : x((T)0), y((T)0), z((T)0), w((T)0) {}
		inline TVector4<T>(T inX, T inY, T inZ, T inW) : x(inX), y(inY), z(inZ), w(inW) {}
		inline TVector4<T>(T* data) : x(data[0]), y(data[1]), z(data[2]), w(data[3]) {}
	};

	typedef TVector4<float> Vector4;
	typedef TVector4<int> Vector4i;
	typedef TVector4<unsigned int> Vector4ui;
}