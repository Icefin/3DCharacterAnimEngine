#pragma once
#include "RigidbodyVolume.h"
#include "Softbody.h"

namespace pa
{
	class PhysicsSystem
	{
	public:
		PhysicsSystem(void);

		void update(float deltaTime);
		void render(Shader& shader);

		void addRigidbody(Rigidbody* body);
		void addSoftbody(Softbody* body);
		void addConstraint(const OBB& constraint);

		void clearRigidbodies(void);
		void clearSoftbodies(void);
		void clearConstraints(void);

	private:
		std::vector<Rigidbody*>			rigidbodies;
		std::vector<Softbody*>			softbodies;;
		std::vector<OBB>				constraints;

		std::vector<Rigidbody*>			colliders1;
		std::vector<Rigidbody*>			colliders2;
		std::vector<CollisionManifold>	collisionManifolds;

		float linearProjectionPercent = 0.45f;
		float penetrationSlack = 0.01f;
		int32 iterationCount = 6;
	};
}