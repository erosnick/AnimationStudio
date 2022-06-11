#pragma once

#include "Shader.h"
#include "Attribute.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Animation/AnimationPose.h"

#include <vector>

#if 0
#include "CCDSolver.h"
#include "FABRIKSolver.h"
#endif

using namespace Math;
using namespace Renderer;
using namespace Animation;

namespace Debug
{
	enum class DebugDrawMode
	{
		Lines, Loop, Strip, Points
	};

	class DebugDraw
	{
	public:
		DebugDraw();
		DebugDraw(unsigned int size);
		~DebugDraw();

		DebugDraw(const DebugDraw&) = delete;
		DebugDraw& operator=(const DebugDraw&) = delete;

		unsigned int Size();
		void Resize(unsigned int newSize);
		Vector3& operator[](unsigned int index);
		void Push(const Vector3& v);

		void UpdateOpenGLBuffers();
		void Draw(DebugDrawMode mode, const Vector3& color, const Matrix4& mvp);

		void FromAnimationPose(const AnimationPose& pose);
	protected:
		std::vector<Vector3> mPoints;
		Attribute<Vector3>* mAttributes;
		Shader* mShader;

#if 0
		void LinesFromIKSolver(CCDSolver& solver);
		void PointsFromIKSolver(CCDSolver& solver);
		void LinesFromIKSolver(FABRIKSolver& solver);
		void PointsFromIKSolver(FABRIKSolver& solver);
#endif
	};
}