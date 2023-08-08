#pragma once
#include "RigidbodyVolume.h"
#include "Cloth.h"

namespace pa
{
	class PhysicsSystem
	{
	protected :
		std::vector<Rigidbody*> bodies;
		std::vector<Cloth*>		clothes;
		std::vector<OBB>		constraints;

		std::vector<Rigidbody*> colliders1;
		std::vector<Rigidbody*> colliders2;
		std::vector<CollisionManifold> collisionManifolds;

		float linearProjectionPercent = 0.45f;
		float penetrationSlack = 0.01f;
		int32 iterationCount = 6;

	public :
		PhysicsSystem(void);

		void update(float deltaTime);
		void render();

		void addRigidbody(Rigidbody* body);
		void addCloth(Cloth* cloth);
		void addConstraint(const OBB& constraint);

		void clearRigidbodys(void);
		void clearClothes(void);
		void clearConstraints(void);
	};
}