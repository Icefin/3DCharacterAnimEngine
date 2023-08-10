#pragma once
#include "Rigidbody.h"

namespace pa
{
	class RigidbodyVolume : public Rigidbody
	{
	public :
		RigidbodyVolume(void) : _mass(1.0f), _friction(0.6f) { _bodyType = RIGIDBODY_TYPE_BASE; }
		RigidbodyVolume(int32 type) : _mass(1.0f), _friction(0.6f) { _bodyType = type; }
		~RigidbodyVolume(void);

		void update(float deltaTime) override;
		void render(Shader& shader) override;
		void applyExternalForces() override;
		void solveConstraints(std::vector<OBB>& constraints) override;

		void	syncCollisionVolumes();

	public :
		float		getInverseMass(void);
		void		applyLinearImpulse(const glm::vec3& impulse);

		glm::vec3	_position;
		glm::vec3	_netForce;
		glm::vec3	_linearVelocity;

	public :
		glm::mat4	getInverseInertiaTensor(void);
		void		applyAngularImpulse(const Point& point, const glm::vec3& impulse);

		glm::vec3	_orientation;
		glm::vec3	_netTorque;
		glm::vec3	_angularVelocity;

	public :
		float		_mass;
		float		_restitution;
		float		_friction;

		OBB			_obb;
		Sphere		_sphere;
	};
}

namespace pa
{
	void	findCollisionManifold(const RigidbodyVolume& ra, const RigidbodyVolume& rb, _Out_ CollisionManifold* outManifold);
	void	applyImpulse(RigidbodyVolume& ra, RigidbodyVolume& rb, const CollisionManifold& manifold, int32 c);
}