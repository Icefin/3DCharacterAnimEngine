#pragma once
#include "Spring.h"
#include "Particle.h"
#include "Shader.h"

#define SOFTBODY_TYPE_BASE	0
#define SOFTBODY_TYPE_CLOTH 1

namespace pa
{
	class Softbody
	{
	public :
		Softbody() { _bodyType = SOFTBODY_TYPE_BASE; }
		virtual ~Softbody() { }

		virtual void update(float deltaTime) { }
		virtual void render(Shader& shader) { }
		virtual void applyInternalForces() { }
		virtual void applyExternalForces() { }
		virtual void solveConstraints(std::vector<OBB>& constraints) { }

		int32 _bodyType = SOFTBODY_TYPE_BASE;
	};
}