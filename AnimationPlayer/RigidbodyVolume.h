#pragma once
#include "Rigidbody.h"

namespace pa
{
	class RigidbodyVolume : public Rigidbody
	{
	public :
		RigidbodyVolume(void) : mass(1.0f), friction(0.6f) { bodyType = RIGIDBODY_TYPE_BASE; }
		RigidbodyVolume(int32 type) : mass(1.0f), friction(0.6f) { bodyType = type; }
		~RigidbodyVolume(void);

		void update(float deltaTime) override;
		void render() override;
		void applyExternalForces() override;
		void solveConstraints() override;

		void	syncCollisionVolumes();

		float		getInverseMass(void);
		glm::mat4	getInverseTensor(void);

		void	applyLinearImpulse(const glm::vec3& impulse);
		void	applyAngularImpulse(const Point& point, const glm::vec3 impulse);

		glm::vec3	position;
		glm::vec3	orientation;

		glm::vec3	linearVelocity;
		glm::vec3	angularVelocity;

		glm::vec3	netForce;
		glm::vec3	netTorque;

		float		mass;
		float		friction;

		OBB			obb;
		Sphere		sphere;
	};
}

namespace pa
{
	CollisionManifold	findCollisionManifold(RigidbodyVolume& ra, RigidbodyVolume& rb);
	void				applyImpulse(RigidbodyVolume& ra, RigidbodyVolume& rb, const CollisionManifold& manifold, int32 c);
}