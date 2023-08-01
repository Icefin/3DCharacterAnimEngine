// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GameObject.h"
#include "Shader.h"
#include "Skeleton.h"
#include "Motion.h"
#include "Animator.h"

#define DEBUG

class Character : public GameObject
{
public:
                                Character(Skeleton* skeleton, std::vector<Motion*>& motionList);
	                            ~Character(void);
    void                        update(Shader& shader, float deltaTime) override;

    void                        move(bool isForward, float deltaTime);
    void                        rotate(bool isClockwise, float deltaTime);

    void                        setCharacterState(AnimationState state);
    AnimationState              getCharacterState(void) { return _currentState; };

private:
    void                        updateMatrixPalette(void);
    void                        renderSkeleton(Shader& shader);

	Skeleton*                   _skeleton = nullptr;
    Animator*                   _animator = nullptr;
    //skinmesh

    std::vector<glm::mat4>      _matrixPalette;
    AnimationState              _currentState = AnimationState::IDLE;

#ifdef DEBUG
    float       axis[12] = {
        0.0f, -17.0f, 10.0f, 1.0f, 0.0f, 0.0f,
        0.0f, -17.0f, 0.0f, 1.0f, 0.0f, 0.0f
    };
#endif // DEBUG
};
