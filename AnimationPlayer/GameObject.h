#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class GameObject
{
public :
	virtual void update(float deltaTime) = 0;
	virtual void render(Shader& shader) = 0;

	uint32		_objectID;
	glm::vec3	_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat	_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
};
