#include "Spring.h"

namespace pa
{
	void Spring::setParticles(Particle* left, Particle* right)
	{
		_left = left;
		_right = right;
	}

	void Spring::setConstants(float stiffness, float damping)
	{
		_stiffness = stiffness;
		_damping = damping;
	}

	void Spring::applySpringForce(float deltaTime)
	{
		glm::vec3 relativePosition = _left->getPosition() - _right->getPosition();
		glm::vec3 relativeVelocity = _left->getVelocity() - _right->getVelocity();

		float stretch = glm::length(relativePosition) - _restLength;

		glm::vec3 springForce = -_stiffness * stretch * glm::normalize(relativePosition);
		glm::vec3 dampingForce = -_damping * relativeVelocity;

		glm::vec3 impulse = (springForce + dampingForce) * deltaTime;

		_left->applyLinearImpulse(impulse);
		_right->applyLinearImpulse(-impulse);
	}
}