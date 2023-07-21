// author: Geonho Shin (icefin@pearlabyss.com)
#include <queue>

#include "Character.h"
#include "CustomMath.h"

uint32 boneBufferObject, boneArrayObject;
uint32 axisBufferObject, axisArrayObject;

Character::Character(Skeleton* skeleton, std::vector<Motion*>& motionList)
{
    _skeleton = skeleton;
    int32 motionSize = motionList.size();
    _motionList.resize(motionSize);
    for (int32 idx = 0; idx < motionSize; ++idx)
        _motionList[idx] = motionList[idx];
    _matrixPalette.resize(_skeleton->getJointNumber());

    //vbo : object vertex set
    //vao : attribute of vertex set
    //ebo : vertex index order of object

    glGenBuffers(1, &boneBufferObject);
    glGenVertexArrays(1, &boneArrayObject);

    glBindVertexArray(boneArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, boneBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bone), bone, GL_STATIC_DRAW); //copy user-defined data into buffer

    // position attribute
    //lcoation = 0, sizeof attrib(vec3), type, want to be normalized?, stride(space btw vertattb.), offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &axisBufferObject);
    glGenVertexArrays(1, &axisArrayObject);

    glBindVertexArray(axisArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, axisBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis), axis, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

Character::~Character()
{
    glDeleteVertexArrays(1, &boneArrayObject);
    glDeleteBuffers(1, &boneBufferObject);

    glDeleteVertexArrays(1, &axisArrayObject);
    glDeleteBuffers(1, &axisBufferObject);

    if (_skeleton != NULL)
        delete _skeleton;

    if (_motionList.empty() == false)
    {
        int32 motionSize = _motionList.size();
        for (int32 i = 0; i < motionSize; ++i)
            delete _motionList[i];
    }
}

void Character::render(Shader& shader, float deltaTime)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(2.0f);

    if (_blendWeight < BLEND_TIME)
        _blendWeight += deltaTime;

    _currentMotionTime += deltaTime;
    if (_currentMotionTime >= _maxFrameTime)
    {
        _currentMotionTime = 0.0f;
        if (_currentState == CharacterState::JUMP)
        {
            setCharacterState(CharacterState::IDLE);
            _isGrounded = true;
        }
    }

    updateMatrixPalette();
    renderSkeleton(shader);
}

void Character::updateMatrixPalette()
{
    glm::quat blendBoneAnimationData = _motionList[static_cast<int32>(_currentState)]->getJointPose(0, _currentMotionTime);
    if (_blendWeight < BLEND_TIME)
    {
        glm::quat prevBoneAnimationData = _motionList[static_cast<int32>(_prevState)]->getJointPose(0, _prevMotionTime);
        blendBoneAnimationData = glm::slerp(prevBoneAnimationData, blendBoneAnimationData, _blendWeight / BLEND_TIME);
    }
    _matrixPalette[0] = glm::mat4(blendBoneAnimationData);

    int32 jointNumber = _skeleton->getJointNumber();
    for (int index = 1; index < jointNumber; ++index)
    {
        Joint* currentJoint = _skeleton->getJoint(index);
        glm::quat blendBoneAnimationData = _motionList[static_cast<int32>(_currentState)]->getJointPose(index, _currentMotionTime);
        if (_blendWeight < BLEND_TIME)
        {
            glm::quat prevBoneAnimationData = _motionList[static_cast<int32>(_prevState)]->getJointPose(index, _prevMotionTime);
            blendBoneAnimationData = glm::slerp(prevBoneAnimationData, blendBoneAnimationData, _blendWeight / BLEND_TIME);
        }

        glm::mat4 matrix = _matrixPalette[currentJoint->parentIndex] * currentJoint->jointToParentMatrix * glm::mat4(blendBoneAnimationData);
        _matrixPalette[index] = matrix;
    }
}

void Character::renderSkeleton(Shader& shader)
{
    int32 jointNumber = _skeleton->getJointNumber();
    for (int index = 0; index < jointNumber; ++index)
    {
        glm::vec3 direction = glm::vec3(10.0, 0.0, 0.0);
        glm::vec3 rotAxis = glm::cross({ 1.0f, 0.0f, 0.0f }, direction);
        float angle = acos(glm::dot({ 1.0f, 0.0f, 0.0f }, glm::normalize(direction)));
        glm::mat4 boneRotation = glm::rotate(glm::mat4(1.0f), angle, rotAxis);

        float scale = glm::length(direction);
        glm::mat4 scaler = glm::scale(glm::mat4(1.0f), { scale, 1.0f, 1.0f });

        glBindVertexArray(boneArrayObject);
        shader.setUniformMat4("model", _matrixPalette[index] * boneRotation * scaler);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(axisArrayObject);
        shader.setUniformMat4("model", _matrixPalette[index]);
        glDrawArrays(GL_LINES, 0, 12);
    }
}

void Character::setCharacterState(CharacterState state)
{
    _prevState = _currentState;
    _currentState = state;
    _maxFrameTime = _motionList[static_cast<int32>(_currentState)]->getMaxFrameTime();

    _blendWeight = BLEND_TIME - _blendWeight;
    _prevMotionTime = _currentMotionTime;
    _currentMotionTime = 0.0f;
    if (state == CharacterState::JUMP)
        _isGrounded = false;
}

void Character::move()
{

}

void Character::rotate()
{

}
