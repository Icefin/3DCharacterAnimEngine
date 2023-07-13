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

constexpr float BLEND_TIME = 30.0f;

enum class CharacterState : uint8
{
    IDLE = 0,
    FORWARD = 1,
    BACKWARD = 2,
    RUN = 3
// JUMP
//    ATTACK = 1 << 5 
};

enum class CharacterBodyLayer : uint8
{
    LOWER_LEFT_LEG = 1 << 1,
    LOWER_RIGHT_LEG = 1 << 2,
    UPPER_BACK = 1 << 3,
    UPPER_LEFT_ARM = 1 << 4,
    UPPER_RIGHT_ARM = 1 << 5,

    LOWER_BODY = LOWER_LEFT_LEG | LOWER_RIGHT_LEG,
    UPPER_BODY = UPPER_LEFT_ARM | UPPER_BACK | UPPER_RIGHT_ARM,
    WHOLE_BODY = LOWER_BODY | UPPER_BODY
};

class Character
{
public:
	                    ~Character();
	void	            initialize(Skeleton* skeleton, std::vector<Motion*>& motionList);
	void	            render(Shader& shader, float deltaTime);

    void                move();
    void                rotate();

    void                setCharacterState(CharacterState state);
    CharacterState      getCharacterState(void) { return _currentState; };

private:
    void                drawBone(Bone* bone, glm::mat4 matrix, Shader& shader);

	Skeleton*                       _skeleton;
	std::vector<Motion*>            _motionList;

    glm::vec3                       _position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3                       _forwardDirection = glm::vec3(1.0f, 0.0f, 0.0f);

    CharacterState                  _prevState = CharacterState::IDLE;
    CharacterState                  _currentState = CharacterState::IDLE;
    float                           _blendWeight = BLEND_TIME;

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
