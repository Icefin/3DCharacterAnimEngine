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

enum class CharacterState : uint8
{
    IDLE = 1 << 0,
    WALK = 1 << 1,
    RUN = 1 << 2,
//    JUMP = 1 << 3,
//    ATTACK = 1 << 4 
};

class Character
{
public:
	        ~Character();
	void	initialize(Skeleton* skeleton, std::vector<Motion*>& motionList);
	void	update(Shader& shader, float deltaTime);

private:
    void    drawBone(Bone* bone, glm::mat4 matrix, Shader& shader);

	Skeleton*                       _skeleton;
	std::vector<Motion*>            _motionList;

    CharacterState                  _prevState = CharacterState::IDLE;
    CharacterState                  _currentState = CharacterState::IDLE;
    float                           _blendWeight = 1.0f;

    float       joint[216] = {
    -0.03f, -0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
     0.03f, -0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
     0.03f,  0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
     0.03f,  0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
    -0.03f,  0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
    -0.03f, -0.03f, -0.03f, 1.0f, 0.0f, 0.0f,

    -0.03f, -0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
     0.03f, -0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
     0.03f,  0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
     0.03f,  0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
    -0.03f,  0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
    -0.03f, -0.03f,  0.03f, 1.0f, 0.0f, 0.0f,

    -0.03f,  0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
    -0.03f,  0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
    -0.03f, -0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
    -0.03f, -0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
    -0.03f, -0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
    -0.03f,  0.03f,  0.03f, 1.0f, 0.0f, 0.0f,

     0.03f,  0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
     0.03f,  0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
     0.03f, -0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
     0.03f, -0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
     0.03f, -0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
     0.03f,  0.03f,  0.03f, 1.0f, 0.0f, 0.0f,

    -0.03f, -0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
     0.03f, -0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
     0.03f, -0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
     0.03f, -0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
    -0.03f, -0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
    -0.03f, -0.03f, -0.03f, 1.0f, 0.0f, 0.0f,

    -0.03f,  0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
     0.03f,  0.03f, -0.03f, 1.0f, 0.0f, 0.0f,
     0.03f,  0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
     0.03f,  0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
    -0.03f,  0.03f,  0.03f, 1.0f, 0.0f, 0.0f,
    -0.03f,  0.03f, -0.03f, 1.0f, 0.0f, 0.0f
    };
    float       bone[216] = {
     0.0f, -0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.5f, -0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.5f,  0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.5f,  0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.0f,  0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     
     0.0f, -0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.5f, -0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.5f,  0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.5f,  0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.0f,  0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.02f,  0.02f, 0.0f, 0.0f, 0.0f,

     0.0f,  0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.0f,  0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.0f,  0.02f,  0.02f, 0.0f, 0.0f, 0.0f,

     0.5f,  0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.5f,  0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.5f, -0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.5f, -0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.5f, -0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.5f,  0.02f,  0.02f, 0.0f, 0.0f, 0.0f,

     0.0f, -0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.5f, -0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.5f, -0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.5f, -0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.02f, -0.02f, 0.0f, 0.0f, 0.0f,

     0.0f,  0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.5f,  0.02f, -0.02f, 0.0f, 0.0f, 0.0f,
     0.5f,  0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.5f,  0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.0f,  0.02f,  0.02f, 0.0f, 0.0f, 0.0f,
     0.0f,  0.02f, -0.02f, 0.0f, 0.0f, 0.0f
    };
    float       axis[36] = {
        0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };
};
