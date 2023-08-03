#pragma once
#include "GameObject.h"

class Sphere : public GameObject
{
public :
	Sphere(glm::vec3 position, float radius, glm::vec3 color = glm::vec3(0.9f, 0.9f, 0.9f), uint32 stackNumber = 20, uint32 sectorNumber = 20);
	~Sphere(void);

	void	update(float deltaTime) override;
	void	render(Shader& shader) override;

private:
	GLuint	_vbo;
	GLuint	_vao;
	GLuint	_ebo;

	int32	_indicesNumber;
};
