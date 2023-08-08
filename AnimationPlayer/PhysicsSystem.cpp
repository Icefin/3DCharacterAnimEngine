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

		int32 numBodies = bodies.size();
		for (int32 i = 0; i < numBodies; ++i)
		{
			for (int32 j = i + 1; j < numBodies; ++j)
			{
				if (i == j)
					continue;

				CollisionManifold manifold;
				resetCollisionManifold(&manifold);

				if (bodies[i]->hasVolume() && bodies[j]->hasVolume())
				{
					RigidbodyVolume* m1 = (RigidbodyVolume*)bodies[i];
					RigidbodyVolume* m2 = (RigidbodyVolume*)bodies[j];
					manifold = findCollisionManifold(*m1, *m2);

					if (manifold.isColliding == true)
					{
						colliders1.push_back(bodies[i]);
						colliders2.push_back(bodies[j]);
						collisionManifolds.push_back(manifold);
					}
				}
			}
		}

		for (int32 i = 0; i < numBodies; ++i)
		{
			bodies[i]->applyInternalForces();
			bodies[i]->applyExternalForces();
		}

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
					RigidbodyVolume* m1 = (RigidbodyVolume*)colliders1[i];
					RigidbodyVolume* m2 = (RigidbodyVolume*)colliders2[i];
					applyImpulse(*m1, *m2, collisionManifolds[i], j);
				}
			}
		}

		for (int32 i = 0; i < numBodies; ++i)
			bodies[i]->update(deltaTime);

		for (int32 i = 0; i < numCollisions; ++i)
		{
			//Linear Projection
		}

		for (int32 i = 0; i < numBodies; ++i)
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
	void PhysicsSystem::addCloth(Cloth* cloth)
	{
		clothes.push_back(cloth);
	}
	void PhysicsSystem::addConstraint(const OBB& constraint)
	{
		constraints.push_back(constraint);
	}

	void PhysicsSystem::clearRigidbodys(void)
	{
		bodies.clear();
	}
	void PhysicsSystem::clearClothes(void)
	{
		clothes.clear();
	}
	void PhysicsSystem::clearConstraints(void)
	{
		constraints.clear();
	}
}