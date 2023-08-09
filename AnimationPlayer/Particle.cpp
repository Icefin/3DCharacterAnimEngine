#include "Particle.h"

namespace pa
{
	Particle::Particle()
	{
		_bodyType = RIGIDBODY_TYPE_PARTICLE;
		_prevPosition = glm::vec3(0.0f, 0.0f, 0.0f);
		_position = glm::vec3(0.0f, 0.0f, 0.0f);
		_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		_netForce = glm::vec3(0.0f, 0.0f, 0.0f);

		_mass = 1.0f;
		_friction = 0.9f;
		_bounciness = 0.7f;
	}

	void Particle::update(float deltaTime)
	{
		_prevPosition = _position;

		glm::vec3 acceleration = getInverseMass() * _netForce;
		_velocity = _friction * _velocity + acceleration * deltaTime;
		_position = _position + _velocity * deltaTime;
	}

	void Particle::render()
	{
		__noop;
	}

	void Particle::applyExternalForces()
	{
		_netForce += _mass * _gravity;
	}

	void Particle::solveConstraints(std::vector<OBB>& constraints)
	{
		int32 n = constraints.size();
		for (int32 i = 0; i < n; ++i)
		{
			Line travelPath(_prevPosition, _position);

			if (isIntersection(travelPath, constraints[i]) == true)
			{
				glm::vec3 velocity = _position - _prevPosition;
				Ray ray(_prevPosition, velocity);

				RaycastInfo raycastInfo;
				raycast(ray, constraints[i], &raycastInfo);
				if (raycastInfo.isHit == true)
				{
					_position = raycastInfo.hitPoint + raycastInfo.normal * 0.003f;

					glm::vec3 vn = raycastInfo.normal * glm::dot(raycastInfo.normal, velocity);
					glm::vec3 vt = velocity - vn;
					//Recheck Here
					_prevPosition = _position - (vt - vn * _bounciness);
				}
			}
		}
	}

	void Particle::applyLinearImpulse(glm::vec3 impulse)
	{
		_velocity += getInverseMass() * impulse;
	}
	
	float Particle::getInverseMass(void)
	{
		if (_mass == 0.0f)
			return 0.0f;

		return (1.0f / _mass);
	}
}