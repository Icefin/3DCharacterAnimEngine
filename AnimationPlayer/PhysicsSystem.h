#pragma once
#include "Rigidbody.h"

namespace pa
{
	class PhysicsSystem
	{
	protected:
		std::vector<Rigidbody*> bodies;
		std::vector<OBB> constraints;

	public:
		void update(float deltaTime);
		void render();

		void addRigidbody(Rigidbody* body);
		void addConstraint(const OBB& constraint);

		void clearRigidbodys();
		void clearConstraints();
	};
}