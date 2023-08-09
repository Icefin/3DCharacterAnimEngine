#include "Cloth.h"

namespace pa
{
	void Cloth::initialize(int32 gridSize, float distance, const glm::vec3& position)
	{
		_sideLength = gridSize;
	}

	void Cloth::setStructuralSprings(float stiffness, float damping)
	{

	}

	void Cloth::setShearSprings(float stiffness, float damping)
	{

	}

	void Cloth::setFlexionSprings(float stiffness, float damping)
	{

	}

	void Cloth::setParticleMass(float mass)
	{

	}

	void Cloth::update(float deltaTime)
	{

	}

	void Cloth::render()
	{

	}

	void Cloth::applyInternalForces()
	{

	}

	void Cloth::applyExternalForces()
	{

	}

	void Cloth::solveConstraints(std::vector<OBB>& constraints)
	{

	}
}