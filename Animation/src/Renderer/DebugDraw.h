#pragma once

#include "Shader.h"
#include "Attribute.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include <vector>

#if 0
#include "Pose.h"
#include "CCDSolver.h"
#include "FABRIKSolver.h"
#endif

using namespace Math;
using namespace Renderer;

enum class DebugDrawMode 
{
	Lines, Loop, Strip, Points
};

class DebugDraw 
{
protected:
	std::vector<Vector3> mPoints;
	Attribute<Vector3>* mAttribs;
	Shader* mShader;
private:
	DebugDraw(const DebugDraw&);
	DebugDraw& operator=(const DebugDraw&);
public:
	DebugDraw();
	DebugDraw(unsigned int size);
	~DebugDraw();

	unsigned int Size();
	void Resize(unsigned int newSize);
	Vector3& operator[](unsigned int index);
	void Push(const Vector3& v);

#if 0
	void FromPose(Pose& pose);
	void LinesFromIKSolver(CCDSolver& solver);
	void PointsFromIKSolver(CCDSolver& solver);
	void LinesFromIKSolver(FABRIKSolver& solver);
	void PointsFromIKSolver(FABRIKSolver& solver);
#endif
	
	void UpdateOpenGLBuffers();
	void Draw(DebugDrawMode mode, const Vector3& color, const Matrix4& mvp);
};