#pragma once
#include <vector>

#include "GameObject.h"

struct MassPoint
{
	float		mass;
	glm::vec3	position;
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
	SpringType type;
	float restLength;
	MassPoint* left;
	MassPoint* right;
};

class PlaneCloth : public GameObject
{
public :
	PlaneCloth(glm::vec3 position, uint32 width, uint32 height, uint32 widthNum, uint32 heightNum);
	~PlaneCloth(void);

	void update(Shader& shader, float deltaTime) override;

private :
	void applyInternalForce(void);
	void applyExternalForce(void);
	void updateMassPointState(float deltaTime);
	void solveCollision(void);
	void render(Shader& shader);

private :
	GLuint	_vao;
	GLuint	_vbo;
	GLuint	_ebo;

	std::vector<MassPoint>	_massPointList;
	std::vector<uint32>		_indices;
	std::vector<Spring>		_springList;
};