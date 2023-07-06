// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Skeleton.h"
#include "Motion.h"

class Character
{
public:
	~Character();
	void	initialize(Skeleton* skeleton, Motion* motion);
	void	draw(Shader& shader, int32 frame);

private:
    void    drawBone(Bone* bone, glm::mat4 matrix, Shader& shader, int32 frame);

	Skeleton*   _skeleton;
	Motion*     _motion;

    float       cube[216] = {
    -0.05f, -0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
     0.05f, -0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
     0.05f,  0.05f, -0.05f, 0.0f, 0.0f, 1.0f,
     0.05f,  0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
    -0.05f,  0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
    -0.05f, -0.05f, -0.05f, 0.0f, 0.0f, 1.0f,

    -0.05f, -0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.05f, 0.0f, 1.0f, 0.0f,
     0.05f,  0.05f,  0.05f, 0.0f, 0.0f, 1.0f,
     0.05f,  0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
    -0.05f,  0.05f,  0.05f, 0.0f, 1.0f, 0.0f,
    -0.05f, -0.05f,  0.05f, 0.0f, 0.0f, 1.0f,

    -0.05f,  0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
    -0.05f,  0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
    -0.05f, -0.05f, -0.05f, 0.0f, 0.0f, 1.0f,
    -0.05f, -0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
    -0.05f, -0.05f,  0.05f, 0.0f, 1.0f, 0.0f,
    -0.05f,  0.05f,  0.05f, 0.0f, 0.0f, 1.0f,

     0.05f,  0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
     0.05f,  0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
     0.05f, -0.05f, -0.05f, 0.0f, 0.0f, 1.0f,
     0.05f, -0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.05f, 0.0f, 1.0f, 0.0f,
     0.05f,  0.05f,  0.05f, 0.0f, 0.0f, 1.0f,

    -0.05f, -0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
     0.05f, -0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
     0.05f, -0.05f,  0.05f, 0.0f, 0.0f, 1.0f,
     0.05f, -0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
    -0.05f, -0.05f,  0.05f, 0.0f, 1.0f, 0.0f,
    -0.05f, -0.05f, -0.05f, 0.0f, 0.0f, 1.0f,

    -0.05f,  0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
     0.05f,  0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
     0.05f,  0.05f,  0.05f, 0.0f, 0.0f, 1.0f,
     0.05f,  0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
    -0.05f,  0.05f,  0.05f, 0.0f, 1.0f, 0.0f,
    -0.05f,  0.05f, -0.05f, 0.0f, 0.0f, 1.0f
    };
};

