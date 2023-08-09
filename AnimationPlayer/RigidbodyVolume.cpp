#include "RigidbodyVolume.h"

namespace pa
{
	RigidbodyVolume::~RigidbodyVolume(void)
	{
		__noop;
	}

	void RigidbodyVolume::update(float deltaTime)
	{
		const float damping = 0.98f;

		glm::vec3 acceleration = _netForce * getInverseMass();
		_linearVelocity = (_linearVelocity + acceleration * deltaTime) * damping;

		_position += _linearVelocity * deltaTime;

		syncCollisionVolumes();
	}

	void RigidbodyVolume::render()
	{
		__noop;
	}

	void RigidbodyVolume::applyExternalForces()
	{
		_netForce += _mass * _gravity;
	}

	void RigidbodyVolume::solveConstraints(std::vector<OBB>& constraints)
	{
		__noop;
	}

	void	RigidbodyVolume::syncCollisionVolumes()
	{
		_obb.position = _position;
		_sphere.position = _position;
	}

	float	RigidbodyVolume::getInverseMass(void)
	{
		if (_mass == 0.0f)
			return 0.0f;

		return 1.0f / _mass;
	}

	glm::mat4	RigidbodyVolume::getInverseInertiaTensor(void)
	{
		float ix = 0.0f;
		float iy = 0.0f;
		float iz = 0.0f;
		float iw = 0.0f;

		if (_mass != 0.0f && _bodyType == RIGIDBODY_TYPE_SPHERE)
		{
			float squareR = _sphere.radius * _sphere.radius;
			float fraction = 2.0f / 5.0f;
			ix = iy = iz = squareR * _mass * fraction;
			iw = 1.0f;
		}
		else if (_mass != 0.0f && _bodyType == RIGIDBODY_TYPE_CUBE)
		{
			glm::vec3 sideLength = _obb.size * 2.0f;
			float fraction = 1.0f / 12.0f;
			float squareX = sideLength.x * sideLength.x;
			float squareY = sideLength.y * sideLength.y;
			float squareZ = sideLength.z * sideLength.z;

			ix = (squareY + squareZ) * _mass * fraction;
			iy = (squareX + squareZ) * _mass * fraction;
			iz = (squareX + squareY) * _mass * fraction;
			iw = 1.0f;
		}

		glm::mat4 inertia(0.0f);
		inertia[0][0] = ix;
		inertia[1][1] = iy;
		inertia[2][2] = iz;
		inertia[3][3] = iw;

		return glm::inverse(inertia);
	}

	void	RigidbodyVolume::applyLinearImpulse(const glm::vec3& impulse)
	{
		_linearVelocity += impulse;
	}

	void	RigidbodyVolume::applyAngularImpulse(const Point& point, const glm::vec3& impulse)
	{
		__noop;
	}
}

namespace pa
{
	void findCollisionManifold(const RigidbodyVolume& ra, const RigidbodyVolume& rb, CollisionManifold* outManifold)
	{
		resetCollisionManifold(outManifold);

		__noop;
	}

	void applyImpulse(RigidbodyVolume& ra, RigidbodyVolume& rb, const CollisionManifold& manifold, int32 c)
	{
		__noop;
	}
}