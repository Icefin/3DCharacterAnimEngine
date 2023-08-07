#include "PhysicsSystem.h"

namespace pa
{
	void PhysicsSystem::update(float deltaTime)
	{
		int32 n = bodies.size();
		for (int32 i = 0; i < n; ++i)
			bodies[i]->applyInternalForces();
		for (int32 i = 0; i < n; ++i)
			bodies[i]->applyExternalForces();
		for (int32 i = 0; i < n; ++i)
			bodies[i]->update(deltaTime);
		for (int32 i = 0; i < n; ++i)
			bodies[i]->solveConstraints();
	}

	void PhysicsSystem::render()
	{
		int32 n = bodies.size();
		for (int32 i = 0; i < n; ++i)
			bodies[i]->render();

		n = constraints.size();
		for (int32 i = 0; i < n; ++i);
			//::render(constraints[i]);
	}

	void PhysicsSystem::addRigidbody(Rigidbody* body)
	{
		bodies.push_back(body);
	}
	void PhysicsSystem::addConstraint(const OBB& constraint)
	{
		constraints.push_back(constraint);
	}

	void PhysicsSystem::clearRigidbodys()
	{
		bodies.clear();
	}
	void PhysicsSystem::clearConstraints()
	{
		constraints.clear();
	}
}