#include "DebugDraw.h"
#include "Uniform.h"
#include "Renderer.h"

namespace Debug
{
	DebugDraw::DebugDraw()
	{
		mAttributes = new Attribute<Vector3>();

		mShader = new Shader("Assets/Shaders/Debug.vert.spv", "Assets/Shaders/Debug.frag.spv");
	}

	DebugDraw::DebugDraw(unsigned int size)
	{
		mAttributes = new Attribute<Vector3>();

		mShader = new Shader("Assets/Shaders/Debug.vert.spv", "Assets/Shaders/Debug.frag.spv");

		Resize(size);
	}

	DebugDraw::~DebugDraw()
	{
		delete mAttributes;
		delete mShader;
	}

	unsigned int DebugDraw::Size()
	{
		return (unsigned int)mPoints.size();
	}

	void DebugDraw::Resize(unsigned int newSize)
	{
		mPoints.resize(newSize);
	}

	Vector3& DebugDraw::operator[](unsigned int index)
	{
		return mPoints[index];
	}

	void DebugDraw::Push(const Vector3& v)
	{
		mPoints.push_back(v);
	}

	void DebugDraw::UpdateOpenGLBuffers()
	{
		mAttributes->set(mPoints);
	}

#if 0
	void DebugDraw::LinesFromIKSolver(CCDSolver& solver)
	{
		if (solver.Size() < 2) { return; }
		unsigned int requiredVerts = (solver.Size() - 1) * 2;
		mPoints.resize(requiredVerts);

		unsigned int index = 0;
		for (unsigned int i = 0, size = solver.Size(); i < size - 1; ++i)
		{
			mPoints[index++] = solver.GetGlobalTransform(i).position;
			mPoints[index++] = solver.GetGlobalTransform(i + 1).position;
		}
	}

	void DebugDraw::PointsFromIKSolver(CCDSolver& solver)
	{
		unsigned int requiredVerts = solver.Size();
		mPoints.resize(requiredVerts);

		for (unsigned int i = 0, size = solver.Size(); i < size; ++i)
		{
			mPoints[i] = solver.GetGlobalTransform(i).position;
		}
	}

	void DebugDraw::LinesFromIKSolver(FABRIKSolver& solver)
	{
		if (solver.Size() < 2) { return; }
		unsigned int requiredVerts = (solver.Size() - 1) * 2;
		mPoints.resize(requiredVerts);

		unsigned int index = 0;
		for (unsigned int i = 0, size = solver.Size(); i < size - 1; ++i)
		{
			mPoints[index++] = solver.GetGlobalTransform(i).position;
			mPoints[index++] = solver.GetGlobalTransform(i + 1).position;
		}
	}

	void DebugDraw::PointsFromIKSolver(FABRIKSolver& solver)
	{
		unsigned int requiredVerts = solver.Size();
		mPoints.resize(requiredVerts);

		for (unsigned int i = 0, size = solver.Size(); i < size; ++i)
		{
			mPoints[i] = solver.GetGlobalTransform(i).position;
		}
	}

#endif

	void DebugDraw::Draw(DebugDrawMode mode, const Vector3& color, const Matrix4& mvp)
	{
		mShader->bind();
		Uniform<Matrix4>::set(mShader->getUniform("mvp"), mvp);
		Uniform<Vector3>::set(mShader->getUniform("color"), color);
		mAttributes->bindTo(mShader->getAttribute("position"));

		if (mode == DebugDrawMode::Lines)
		{
			::draw(Size(), RenderMode::Lines);
		}
		else if (mode == DebugDrawMode::Loop)
		{
			::draw(Size(), RenderMode::LineLoop);
		}
		else if (mode == DebugDrawMode::Strip)
		{
			::draw(Size(), RenderMode::LineStrip);
		}
		else
		{
			::draw(Size(), RenderMode::Points);
		}

		mAttributes->unbindFrom(mShader->getAttribute("position"));
		mShader->unbind();
	}

	void DebugDraw::FromAnimationPose(const AnimationPose& pose)
	{
		unsigned int requiredVerts = 0;
		unsigned int numJoints = pose.getSize();
		for (unsigned int i = 0; i < numJoints; ++i)
		{
			if (pose.getParent(i) < 0) {
				continue;
			}

			requiredVerts += 2;
		}

		mPoints.resize(requiredVerts);
		
		for (unsigned int i = 0; i < numJoints; ++i)
		{
			if (pose.getParent(i) < 0) {
				continue;
			}

			mPoints.push_back(pose.getGlobalTransform(i).position);
			mPoints.push_back(pose.getGlobalTransform(pose.getParent(i)).position);
		}
	}
}