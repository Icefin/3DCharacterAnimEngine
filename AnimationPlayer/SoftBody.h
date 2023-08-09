#pragma once
#include "Spring.h"
#include "Particle.h"

namespace pa
{
	class Softbody
	{
	public :
		virtual void update(float deltaTime);
		virtual void render();
		virtual void applyInternalForces();
		virtual void applyExternalForces();
		virtual void solveConstraints(std::vector<OBB>& constraints);

	protected :
		glm::vec3 _gravity = glm::vec3(0.0f, -9.81f, 0.0f);
	};
}