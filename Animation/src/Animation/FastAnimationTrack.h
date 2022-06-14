#pragma once

#include "AnimationTrack.h"

#include <cstdint>
#include <vector>

namespace Animation
{
	template <typename T, int32_t N>
	class FastAnimationTrack : public AnimationTrack<T, N>
	{
	public:
		void updateIndexLookupTable();
	protected:
		virtual int32_t frameIndex(float time, bool bLooping) override;
		
	protected:
		std::vector<uint32_t> sampledFrames;
	};

	using FastScalarTrack = FastAnimationTrack<float, 1>;
	using FastVectorTrack = FastAnimationTrack<Vector3, 3>;
	using FastQuaternionTrack = FastAnimationTrack<Quaternion, 4>;

	template <typename T, int32_t N>
	FastAnimationTrack<T, N> optimizeAnimationTrack(AnimationTrack<T, N>& input);
}