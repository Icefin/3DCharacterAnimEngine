#pragma once
#include <vector>

#include "Geometry3D.h"

namespace pa
{
	class Rigidbody
	{
	public:
		Rigidbody() { }
		virtual ~Rigidbody() { }

		virtual void update(float deltaTime) { }
		virtual void render() { }
		virtual void applyInternalForces() { }
		virtual void applyExternalForces() { }
		virtual void solveConstraints() { }

	private:

	};
}