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

enum class CharacterBodyMask : uint32
{
    ROOT = 1 << 0,

    LEFT_HIP_JOINT = 1 << 1,
    LEFT_FEMUR = 1 << 2,
    LEFT_TIBIA = 1 << 3,
    LEFT_FOOT = 1 << 4,
    LEFT_TOES = 1 << 5,

    RIGHT_HIT_JOINT = 1 << 6,
    RIGHT_FEMUR = 1 << 7,
    RIGHT_TIBIA = 1 << 8,
    RIGHT_FOOT = 1 << 9,
    RIGHT_TOES = 1 << 10,

    LOWER_BACK = 1 << 11,
    UPPER_BACK = 1 << 12,
    THROAX = 1 << 13,
    LOWER_NECK = 1 << 14,
    UPPER_NECK = 1 << 15,
    HEAD = 1 << 16,

    LEFT_CLAVICLE = 1 << 17,
    LEFT_HUMERUS = 1 << 18,
    LEFT_RADIUS = 1 << 19,
    LEFT_WRIST = 1 << 20,
    LEFT_HAND = 1 << 21,
    LEFT_FINGERS = 1 << 22,
    LEFT_THUMB = 1 << 23,

    RIGHT_CLAVICLE = 1 << 24,
    RIGHT_HUMERUS = 1 << 25,
    RIGHT_RADIUS = 1 << 26,
    RIGHT_WRIST = 1 << 27,
    RIGHT_HAND = 1 << 28,
    RIGHT_FINGERS = 1 << 29,
    RIGHT_THUMB = 1 << 30,

    LEFT_LEG = LEFT_HIP_JOINT | LEFT_FEMUR | LEFT_TIBIA | LEFT_FOOT | LEFT_TOES,
    RIGHT_LEG = RIGHT_HIT_JOINT | RIGHT_FEMUR | RIGHT_TIBIA | RIGHT_FOOT | RIGHT_TOES,
    SPINE = LOWER_BACK | UPPER_BACK | THROAX | LOWER_NECK | UPPER_NECK | HEAD,
    LEFT_ARM = LEFT_CLAVICLE | LEFT_HUMERUS | LEFT_RADIUS | LEFT_WRIST | LEFT_HAND | LEFT_FINGERS | LEFT_THUMB,
    RIGHT_ARM = RIGHT_CLAVICLE | RIGHT_HUMERUS | RIGHT_RADIUS | RIGHT_WRIST | RIGHT_HAND | RIGHT_FINGERS | RIGHT_THUMB,

    LOWER_BODY = LEFT_LEG | RIGHT_LEG,
    UPPER_BODY = SPINE | LEFT_ARM | RIGHT_ARM,

    WHOLE_BODY = LOWER_BODY | UPPER_BODY
};
//Give prev, current state to all bones...?

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
