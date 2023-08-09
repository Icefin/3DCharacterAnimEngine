#pragma once
#include <vector>

#include "Geometry3D.h"

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
		virtual void render() { }
		virtual void applyExternalForces() { }
		virtual void solveConstraints(std::vector<OBB>& constraints) { }

		bool hasVolume(void) { return (_bodyType == RIGIDBODY_TYPE_SPHERE || _bodyType == RIGIDBODY_TYPE_CUBE); }

	protected :
		int32 _bodyType = RIGIDBODY_TYPE_BASE;

		glm::vec3 _gravity = glm::vec3(0.0f, -9.81f, 0.0f);
	};
}