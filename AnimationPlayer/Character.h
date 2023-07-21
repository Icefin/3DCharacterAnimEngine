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
//  JUMP = 4,
//  ATTACK = 5 
};

class Character
{
public:
                        Character(Skeleton* skeleton, std::vector<Motion*>& motionList);
	                    ~Character();
	void	            render(Shader& shader, float deltaTime);

    void                move();
    void                rotate();

    void                setCharacterState(CharacterState state);
    CharacterState      getCharacterState(void) { return _currentState; };

private:
	Skeleton*                       _skeleton;
	std::vector<Motion*>            _motionList;
    //skinmesh

    std::vector<glm::mat4>          _matrixPalette;
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
     1.0f, -0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     1.0f, -0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     1.0f,  0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     1.0f,  0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     0.0f,  0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     
     0.0f, -0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     1.0f, -0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     1.0f,  0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     1.0f,  0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     0.0f,  0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.2f,  0.2f, 0.0f, 0.0f, 0.0f,

     0.0f,  0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     0.0f,  0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     0.0f,  0.2f,  0.2f, 0.0f, 0.0f, 0.0f,

     1.0f,  0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     1.0f,  0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     1.0f, -0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     1.0f, -0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     1.0f, -0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     1.0f,  0.2f,  0.2f, 0.0f, 0.0f, 0.0f,

     0.0f, -0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     1.0f, -0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     1.0f, -0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     1.0f, -0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.2f, -0.2f, 0.0f, 0.0f, 0.0f,

     0.0f,  0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     1.0f,  0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
     1.0f,  0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     1.0f,  0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     0.0f,  0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
     0.0f,  0.2f, -0.2f, 0.0f, 0.0f, 0.0f
    };
    float       axis[36] = {
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };
};
