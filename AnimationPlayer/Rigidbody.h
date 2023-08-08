#pragma once
#include <vector>

#include "Geometry3D.h"

#define RIGIDBODY_TYPE_BASE		0
#define RIGIDBODY_TYPE_PARTICLE 1
#define RIGIDBODY_TYPE_SPHERE	2
#define RIGIDBODY_TYPE_CUBE		3
#define RIGIDBODY_TYPE_CYLINDER 4

namespace pa
{
	class Rigidbody
	{
	public :
		Rigidbody() { }
		virtual ~Rigidbody() { }

		virtual void update(float deltaTime) { }
		virtual void render() { }
		virtual void applyInternalForces() { }
		virtual void applyExternalForces() { }
		virtual void solveConstraints() { }

		int32 bodyType = RIGIDBODY_TYPE_BASE;
		bool hasVolume(void) { return (bodyType == RIGIDBODY_TYPE_SPHERE || bodyType == RIGIDBODY_TYPE_CUBE || bodyType == RIGIDBODY_TYPE_CYLINDER); }

	private :

	};
}