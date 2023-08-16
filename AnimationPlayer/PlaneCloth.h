#pragma once
#include <vector>

#include "Geometry3D.h"
#include "GameObject.h"

struct MassPoint
{
	float		invMass;
	glm::vec3	prevPosition;
	glm::vec3	position;
	glm::vec3	normal;
	glm::vec3	velocity;
	glm::vec3	color;
};

struct CollisionConstraint
{
	glm::vec3	targetPosition;
	MassPoint*	point;
};

struct DistanctConstraint
{
	float		restLength;
	MassPoint*	left;
	MassPoint*	right;
};

class PlaneCloth : public GameObject
{
public:
	PlaneCloth(glm::vec3 position, glm::vec3 color, uint32 width, uint32 height, uint32 widthNum, uint32 heightNum);
	~PlaneCloth(void);
	
	void update(float deltaTime) { }
	void update(float deltaTime, std::vector<pa::OBB>& colliders);
	void render(Shader& shader) override;

private:
	void generateCollisionConstraint(MassPoint& massPoint, std::vector<pa::OBB> colliders, std::vector<CollisionConstraint>* collisionConstraints);
	void solveDistantConstraint(DistanctConstraint& constraint, float deltaTime);
	void solveCollisionConstraint(CollisionConstraint& constraint, float deltaTime);
	void updateMassPointNormal(void);

private:
	GLuint	_vao;
	GLuint	_vbo;
	GLuint	_ebo;

	std::vector<MassPoint>			_massPointList;
	std::vector<uint32>				_indices;
	std::vector<DistanctConstraint> _internalConstraints;

private:
	glm::vec3	_materialAmbient{0.1f, 0.1f, 0.1f};
	glm::vec3	_materialSpecular{0.2f, 0.2f, 0.2f};
	glm::vec3	_materialEmissive{0.0f, 0.0f, 0.0f};
	float		_materialShininess = 5.0f;
};