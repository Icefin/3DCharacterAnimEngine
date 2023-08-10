#pragma once
#include <vector>

#include "Geometry3D.h"
#include "Shader.h"

#define RIGIDBODY_TYPE_BASE		0
#define RIGIDBODY_TYPE_PARTICLE 1
#define RIGIDBODY_TYPE_SPHERE	2
#define RIGIDBODY_TYPE_CUBE		3

namespace pa
{
	class Rigidbody
	{
	public :
		Rigidbody() { _bodyType = RIGIDBODY_TYPE_BASE; }
		virtual ~Rigidbody() { }

		virtual void update(float deltaTime) { }
		virtual void render(Shader& shader) { }
		virtual void applyExternalForces() { }
		virtual void solveConstraints(std::vector<OBB>& constraints) { }

		bool hasVolume(void) { return (_bodyType == RIGIDBODY_TYPE_SPHERE || _bodyType == RIGIDBODY_TYPE_CUBE); }

		int32 _bodyType = RIGIDBODY_TYPE_BASE;
	};
}