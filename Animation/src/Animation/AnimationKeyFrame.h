#pragma once

#include <cstdint>

namespace Animation
{
	template <uint32_t N>
	class AnimationKeyFrame
	{
	public:
		float value[N];
		float in[N];
		float out[N];
		float time;
	};

	using ScalarKeyFrame = AnimationKeyFrame<1>;
	using VectorKeyFrame = AnimationKeyFrame<2>;
	using QuaternionKeyFrame = AnimationKeyFrame<4>;
}