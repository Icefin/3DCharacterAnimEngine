#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class GameObject
{
public :
	glm::vec3	_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3	_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
};
