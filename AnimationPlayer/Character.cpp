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

    glGenBuffers(1, &axisBufferObject);
    glGenVertexArrays(1, &axisArrayObject);

    glBindVertexArray(axisArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, axisBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis), axis, GL_STATIC_DRAW);
    // position attribute
    //lcoation = 0, sizeof attrib(vec3), type, want to be normalized?, stride(space btw vertattb.), offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
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

void Character::render(Shader& shader)
{
    __noop;
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
    for (int32 index = 1; index < jointNumber; ++index)
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
    for (int32 index = 1; index < jointNumber; ++index)
    {
        Joint* currentJoint = _skeleton->getJoint(index);
        glm::vec3 currentPosition = glm::vec3(_matrixPalette[index] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        glm::vec3 parentPosition = glm::vec3(_matrixPalette[currentJoint->parentIndex] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        float vertices[12] = {
            currentPosition.x, currentPosition.y, currentPosition.z, 0.0f, 0.0f, 0.0f,
            parentPosition.x, parentPosition.y, parentPosition.z, 0.0f, 0.0f, 0.0f
        };

        GLuint testBufferObject;
        GLuint testArrayObject;

        glGenBuffers(1, &testBufferObject);
        glGenVertexArrays(1, &testArrayObject);

        glBindVertexArray(testArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, testBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(testArrayObject);
        shader.setUniformMat4("model", glm::mat4(1.0f));
        glLineWidth(8.0f);
        glDrawArrays(GL_LINES, 0, 6);
        
        glDeleteVertexArrays(1, &testArrayObject);
        glDeleteBuffers(1, &testBufferObject);

        glBindVertexArray(axisArrayObject);
        shader.setUniformMat4("model", _matrixPalette[index]);
        glLineWidth(2.0f);
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
