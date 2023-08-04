#pragma once
#include <vector>

#include "GameObject.h"

struct MassPoint
{
	float		mass;
	glm::vec3	position;
	glm::vec3	normal;
	glm::vec3	velocity;
	glm::vec3	netForce;
	glm::vec3	color;
};

enum class SpringType :uint8
{
	Structural = 0,
	Shear,
	Flexion
};

struct Spring
{
	SpringType	type;
	float		restLength;
	MassPoint*	left;
	MassPoint*	right;
};

class PlaneCloth : public GameObject
{
public:
	PlaneCloth(glm::vec3 position, uint32 width, uint32 height, uint32 widthNum, uint32 heightNum, bool test);
	~PlaneCloth(void);

	void update(float deltaTime) override;
	void render(Shader& shader) override;

private:
	void applyInternalForces(void);
	void applyExternalForces(void);
	void updateMassPointState(float deltaTime);
	void solveCollision(void);
	void updateMassPointNormal(void);

private:
	GLuint	_vao;
	GLuint	_vbo;
	GLuint	_ebo;

	std::vector<MassPoint>	_massPointList;
	std::vector<uint32>		_indices;
	std::vector<Spring>		_springList;

	bool _test;

private:
	glm::vec3	_materialAmbient{0.0f, 0.0f, 0.0f};
	glm::vec3	_materialSpecular{0.2f, 0.2f, 0.2f};
	glm::vec3	_materialDiffuse{0.1f, 0.1f, 0.1f};
	float		_materialShininess = 5.0f;
};