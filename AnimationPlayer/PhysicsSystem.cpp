#include "PhysicsSystem.h"

namespace pa
{
	PhysicsSystem::PhysicsSystem()
	{
		linearProjectionPercent = 0.45f;
		penetrationSlack = 0.01f;
		iterationCount = 6;

		colliders1.reserve(100);
		colliders2.reserve(100);
		collisionManifolds.reserve(100);
	}

	void PhysicsSystem::update(float deltaTime)
	{
		colliders1.clear();
		colliders2.clear();
		collisionManifolds.clear();

		int32 numBodies = rigidbodies.size();
		for (int32 i = 0; i < numBodies; ++i)
		{
			for (int32 j = i + 1; j < numBodies; ++j)
			{
				if (i == j)
					continue;

				if (rigidbodies[i]->hasVolume() && rigidbodies[j]->hasVolume())
				{
					CollisionManifold manifold;
					RigidbodyVolume* m1 = static_cast<RigidbodyVolume*>(rigidbodies[i]);
					RigidbodyVolume* m2 = static_cast<RigidbodyVolume*>(rigidbodies[j]);
					findCollisionManifold(*m1, *m2, &manifold);

					if (manifold.isColliding == true)
					{
						colliders1.push_back(rigidbodies[i]);
						colliders2.push_back(rigidbodies[j]);
						collisionManifolds.push_back(manifold);
					}
				}
			}
		}

		for (int32 i = 0; i < numBodies; ++i)
			rigidbodies[i]->applyExternalForces();

		/*int32 numClothes = clothes.size();
		for (int32 i = 0; i < numClothes; ++i)
		{
			clothes[i]->applyInternalForces();
			clothes[i]->applyExternalForces();
		}*/

		int32 numCollisions = collisionManifolds.size();
		for (int32 cnt = 0; cnt < iterationCount; ++cnt)
		{
			for (int32 i = 0; i < numCollisions; ++i)
			{
				int32 numContacts = collisionManifolds[i].contacts.size();
				for (int32 j = 0; j < numContacts; ++j) 
				{
					RigidbodyVolume* m1 = static_cast<RigidbodyVolume*>(colliders1[i]);
					RigidbodyVolume* m2 = static_cast<RigidbodyVolume*>(colliders2[i]);
					applyImpulse(*m1, *m2, collisionManifolds[i], j);
				}
			}
		}

		for (int32 i = 0; i < numBodies; ++i)
			rigidbodies[i]->update(deltaTime);

		for (int32 i = 0; i < numCollisions; ++i)
		{
			//Linear Projection
		}

		int32 numSprings = springs.size();
		for (int32 i = 0; i < numSprings; ++i)
			springs[i].applyForce(deltaTime);

		for (int32 i = 0; i < numBodies; ++i)
			rigidbodies[i]->solveConstraints(constraints);
	}

	void PhysicsSystem::render(Shader& shader)
	{
		int32 n = rigidbodies.size();
		for (int32 i = 0; i < n; ++i)
			rigidbodies[i]->render();

		n = constraints.size();
		for (int32 i = 0; i < n; ++i);
			//::render(constraints[i]);
	}

	void PhysicsSystem::addRigidbody(Rigidbody* body)
	{
		rigidbodies.push_back(body);
	}
	void PhysicsSystem::addSoftbody(Softbody* body)
	{
		softbodies.push_back(body);
	}
	void PhysicsSystem::addConstraint(const OBB& constraint)
	{
		constraints.push_back(constraint);
	}

	void PhysicsSystem::clearRigidbodies(void)
	{
		rigidbodies.clear();
	}
	void PhysicsSystem::clearSoftbodies(void)
	{
		softbodies.clear();
	}
	void PhysicsSystem::clearConstraints(void)
	{
		constraints.clear();
	}

	void PhysicsSystem::addSpring(const Spring& spring)
	{
		springs.push_back(spring);
	}

	void PhysicsSystem::clearSpring(void)
	{
		springs.clear();
	}
}