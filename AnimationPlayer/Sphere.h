#pragma once
#include "GameObject.h"

class Sphere : public GameObject
{
	Sphere(glm::vec3 position, float radius, uint32 stackNumber = 20, uint32 sectorNumber = 20, glm::vec3 color = glm::vec3(0.9f, 0.9f, 0.9f));
	~Sphere(void);

	void	update(Shader& shader, float deltaTime) override;

private:
	GLuint	_vbo;
	GLuint	_vao;
	GLuint	_ebo;

	int32	_indicesNumber;
};
