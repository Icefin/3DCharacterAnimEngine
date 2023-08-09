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

		glm::vec3 acceleration = netForce * getInverseMass();
		linearVelocity = (linearVelocity + acceleration * deltaTime) * damping;

		position += linearVelocity * deltaTime;

		syncCollisionVolumes();
	}

	void RigidbodyVolume::render()
	{
		__noop;
	}

	void RigidbodyVolume::applyExternalForces()
	{
		netForce += glm::vec3(0.0f, -9.81f, 0.0f) * mass;
	}

	void RigidbodyVolume::solveConstraints()
	{
		__noop;
	}

	void	RigidbodyVolume::syncCollisionVolumes()
	{
		obb.position = position;
		sphere.position = position;
	}

	float	RigidbodyVolume::getInverseMass(void)
	{
		if (mass == 0.0f)
			return 0.0f;

		return 1.0f / mass;
	}

	glm::mat4	RigidbodyVolume::getInverseTensor(void)
	{
		__noop;
		return glm::mat4(1.0f);
	}

	void	RigidbodyVolume::applyLinearImpulse(const glm::vec3& impulse)
	{
		linearVelocity += impulse;
	}

	void	RigidbodyVolume::applyAngularImpulse(const Point& point, const glm::vec3 impulse)
	{
		__noop;
	}
}

namespace pa
{
	CollisionManifold findCollisionManifold(RigidbodyVolume& ra, RigidbodyVolume& rb)
	{
		CollisionManifold manifold;
		resetCollisionManifold(&manifold);

		__noop;

		return manifold;
	}

	void applyImpulse(RigidbodyVolume& ra, RigidbodyVolume& rb, const CollisionManifold& manifold, int32 c)
	{
		__noop;
	}
}