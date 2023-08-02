// author: Geonho Shin (icefin@pearlabyss.com)
#include <queue>

#include "Character.h"
#include "CustomMath.h"

#ifdef DEBUG
    uint32 axisBufferObject, axisArrayObject;
#endif // DEBUG

Character::Character(Skeleton* skeleton, std::vector<Motion*>& motionList)
{
    _skeleton = skeleton;
    _animator = new Animator(motionList);

    _matrixPalette.resize(_skeleton->getJointNumber());

#ifdef DEBUG
    glGenVertexArrays(1, &axisArrayObject);
    glBindVertexArray(axisArrayObject);

    glGenBuffers(1, &axisBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, axisBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis), axis, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif // DEBUG
}

Character::~Character()
{
#ifdef DEBUG
    glDeleteVertexArrays(1, &axisArrayObject);
    glDeleteBuffers(1, &axisBufferObject);
#endif // DEBUG

    if (_skeleton != NULL)
        delete _skeleton;

    if (_animator != NULL)
        delete _animator;
}

void Character::update(Shader& shader, float deltaTime)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(2.0f);

    _animator->update(_currentState, deltaTime);
    updateMatrixPalette();
    renderSkeleton(shader);

    glBindVertexArray(axisArrayObject);
    shader.setUniformMat4("model", _matrixPalette[0]);
    glLineWidth(5.0f);
    glDrawArrays(GL_LINES, 0, 12);
}

void Character::updateMatrixPalette()
{
    glm::mat4 rootMatrix = glm::translate(glm::mat4(1.0f), _position);
    rootMatrix = glm::rotate(rootMatrix, glm::radians(-_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    _matrixPalette[0] = rootMatrix /* glm::mat4(_animator->getJointAnimation(0)) */;

    int32 jointNumber = _skeleton->getJointNumber();
    for (int32 index = 1; index < jointNumber; ++index)
    {
        glm::quat blendBoneAnimationData = _animator->getJointAnimation(index);

        Joint* currentJoint = _skeleton->getJoint(index);
        glm::mat4 matrix = _matrixPalette[currentJoint->parentIndex] * currentJoint->jointToParentMatrix * glm::mat4(blendBoneAnimationData);
        _matrixPalette[index] = matrix;
    }
}

void Character::renderSkeleton(Shader& shader)
{
    int32 jointNumber = _skeleton->getJointNumber();
    for (int32 index = 1; index < jointNumber; ++index)
    {
        Joint* currentJoint = _skeleton->getJoint(index);
        glm::vec3 currentPosition = glm::vec3(_matrixPalette[index] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        glm::vec3 parentPosition = glm::vec3(_matrixPalette[currentJoint->parentIndex] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        float vertices[12] = {
            currentPosition.x, currentPosition.y, currentPosition.z, 0.0f, 0.0f, 0.0f,
            parentPosition.x, parentPosition.y, parentPosition.z, 0.0f, 0.0f, 0.0f
        };

        GLuint boneBufferObject;
        GLuint boneArrayObject;

        glGenVertexArrays(1, &boneArrayObject);
        glBindVertexArray(boneArrayObject);

        glGenBuffers(1, &boneBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, boneBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

        shader.setUniformMat4("model", glm::mat4(1.0f));
        glLineWidth(8.0f);
        glDrawArrays(GL_LINES, 0, 6);
        
        glDeleteVertexArrays(1, &boneArrayObject);
        glDeleteBuffers(1, &boneBufferObject);

#ifdef DEBUG_JOINT
        glBindVertexArray(axisArrayObject);
        shader.setUniformMat4("model", _matrixPalette[index]);
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, 12);
#endif // DEBUG
    }
}

void Character::setCharacterState(AnimationState state)
{
    _currentState = state;
}

void Character::move(bool isForward, float deltaTime)
{
    static const float kForwardSpeed= 0.2f;
    static const float kRunSpeed = 0.6f;
    static const float kBackwardSpeed = 0.1f;

    glm::vec3 offset = glm::vec3(-glm::sin(glm::radians(_rotation.y)), 0.0f, glm::cos(glm::radians(_rotation.y)));
    switch (_currentState)
    {
    case AnimationState::FORWARD :
        offset *= kForwardSpeed;
        break;
    case AnimationState::RUN :
        offset *= kRunSpeed;
        break;
    case AnimationState::BACKWARD :
        offset *= kBackwardSpeed;
        break;
    }
    _position += isForward ? offset : -offset;
}

void Character::rotate(bool isClockwise, float deltaTime)
{
    const static float kCharacterAngularSpeed = 5.0f;

    _rotation.y += isClockwise ? kCharacterAngularSpeed : -kCharacterAngularSpeed;
    if (_rotation.y > 360.0f || _rotation.y < -360.0f)
        _rotation.y = 0.0f;
}
