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

		glm::vec3 angularAcceleration = glm::vec3(getInverseInertiaTensor() * glm::vec4(_netTorque, 1.0f));
		_angularVelocity = (_angularVelocity + angularAcceleration * deltaTime) * damping;
		_orientation += _angularVelocity * deltaTime;

		syncCollisionVolumes();
	}

	void RigidbodyVolume::render(Shader& shader)
	{
		__noop;
	}

	void RigidbodyVolume::applyExternalForces()
	{
		_netForce += _mass * gravity;
	}

	void RigidbodyVolume::solveConstraints(std::vector<OBB>& constraints)
	{
		__noop;
	}

	void	RigidbodyVolume::syncCollisionVolumes()
	{
		_sphere.position = _position;

		_obb.position = _position;
		//TODO
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
		Point centerOfMass = _position;
		glm::vec3 torque = glm::cross(point - centerOfMass, impulse);

		glm::vec3 angularAcceleration = glm::vec3(getInverseInertiaTensor() * glm::vec4(torque, 1.0f));
		_angularVelocity = _angularVelocity + angularAcceleration;
	}
}

namespace pa
{
	void findCollisionManifold(const RigidbodyVolume& ra, const RigidbodyVolume& rb, CollisionManifold* outManifold)
	{
		resetCollisionManifold(outManifold);

		if (ra._bodyType == RIGIDBODY_TYPE_SPHERE)
		{
			if (rb._bodyType == RIGIDBODY_TYPE_SPHERE)
				findCollisionManifold(ra._sphere, rb._sphere, outManifold);
			else if (rb._bodyType == RIGIDBODY_TYPE_CUBE)
			{
				findCollisionManifold(rb._obb, ra._sphere, outManifold);
				outManifold->normal = outManifold->normal * -1.0f;
			}
		}
		else if (ra._bodyType == RIGIDBODY_TYPE_CUBE)
		{
			if (rb._bodyType == RIGIDBODY_TYPE_SPHERE)
				findCollisionManifold(ra._obb, rb._sphere, outManifold);
			else if (rb._bodyType == RIGIDBODY_TYPE_CUBE)
				findCollisionManifold(ra._obb, rb._obb, outManifold);
		}
	}

	void applyImpulse(RigidbodyVolume& ra, RigidbodyVolume& rb, const CollisionManifold& manifold, int32 c)
	{
		float raInvMass = ra.getInverseMass();
		float rbInvMass = rb.getInverseMass();
		float invMassSum = raInvMass + rbInvMass;

		if (invMassSum == 0.0f)
			return;

		glm::vec3 r1 = manifold.contacts[c] - ra._position;
		glm::vec3 r2 = manifold.contacts[c] - rb._position;
		glm::mat4 raInvInertia = ra.getInverseInertiaTensor();
		glm::mat4 rbInvInertia = rb.getInverseInertiaTensor();

		glm::vec3 relativeVelocity = (rb._linearVelocity + glm::cross(rb._angularVelocity, r2)) - (ra._linearVelocity + glm::cross(ra._angularVelocity, r1));
		glm::vec3 relativeNormal = manifold.normal;

		if (glm::dot(relativeVelocity, relativeNormal) > 0.0f)
			return;

		float e = fminf(ra._restitution, rb._restitution);

		float numerator = (-(1.0f + e) * glm::dot(relativeVelocity, relativeNormal));
		float d1 = invMassSum;
		
		//TODO
	}
}