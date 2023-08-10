#pragma once

#include "Rigidbody.h"

namespace pa
{
	class Particle : public Rigidbody
	{
	public :
		Particle();

		void update(float deltaTime) override;
		void render(Shader& shader) override;
		void applyExternalForces() override;
		void solveConstraints(std::vector<OBB>& constraints) override;

		void setPosition(glm::vec3& position) { _position = position; }
		void setMass(float mass) { _mass = mass; }
		void setFriction(float friction) { _friction = friction; }
		void setBounciness(float bounciness) { _bounciness = bounciness; }

		glm::vec3	getPosition(void) { return _position; }
		glm::vec3	getVelocity(void) { return _velocity; }
		float		getInverseMass(void);
		float		getBounciness(void) { return _bounciness; }

	private :
		glm::vec3 _prevPosition;
		glm::vec3 _position;
		glm::vec3 _velocity;
		glm::vec3 _netForce;

		float _mass;
		float _friction;
		float _bounciness;
	};
}

