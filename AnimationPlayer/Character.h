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

constexpr float BLEND_TIME = 30.0f;

enum class CharacterState : uint8
{
    IDLE = 0,
    FORWARD = 1,
    BACKWARD = 2,
    RUN = 3,
    JUMP = 4,
//  ATTACK = 5 
};

class Character : public GameObject
{
public:
                        Character(Skeleton* skeleton, std::vector<Motion*>& motionList);
	                    ~Character(void);
	void	            render(Shader& shader, float deltaTime);
    void                render(Shader& shader) override;
    void                updateMatrixPalette(void);

    void                move();
    void                rotate();

    void                setCharacterState(CharacterState state);
    CharacterState      getCharacterState(void) { return _currentState; };
    bool                isGrounded(void) { return _isGrounded; };

private:
    void                renderSkeleton(Shader& shader);
	Skeleton*                       _skeleton;
	std::vector<Motion*>            _motionList;
    //skinmesh

    bool                            _isGrounded = true;

    std::vector<glm::mat4>          _matrixPalette;
    CharacterState                  _prevState = CharacterState::IDLE;
    float                           _prevMotionTime = 0.0f;
    CharacterState                  _currentState = CharacterState::IDLE;
    float                           _currentMotionTime = 0.0f;
    float                           _blendWeight = BLEND_TIME;
    int32                           _maxFrameTime = 0;

    float       axis[36] = {
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };
};
