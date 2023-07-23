#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class GameObject
{
public :
	virtual void render(Shader& shader) = 0;

protected :
	int32		_objectID;
	glm::vec3	_position;
	glm::quat	_rotation;
};
