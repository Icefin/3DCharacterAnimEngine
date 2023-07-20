// author: Geonho Shin (icefin@pearlabyss.com)
#include <queue>

#include "Character.h"
#include "Transform.h"

uint32 boneBufferObject, boneArrayObject;
uint32 jointBufferObject, jointArrayObject;

Character::~Character()
{
    glDeleteVertexArrays(1, &boneArrayObject);
    glDeleteBuffers(1, &boneBufferObject);

    glDeleteVertexArrays(1, &jointArrayObject);
    glDeleteBuffers(1, &jointBufferObject);

    if (_skeleton != NULL)
        delete _skeleton;

    if (_motionList.empty() == false)
    {
        int32 motionSize = _motionList.size();
        for (int32 i = 0; i < motionSize; ++i)
            delete _motionList[i];
    }
}

void Character::initialize(Skeleton* skeleton, std::vector<Motion*>& motion)
{
	_skeleton = skeleton;
    int32 motionSize = motion.size();
    _motionList.resize(motionSize);
    for (int32 idx = 0; idx < motionSize; ++idx)
        _motionList[idx] = motion[idx];

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

    glGenBuffers(1, &jointBufferObject);
    glGenVertexArrays(1, &jointArrayObject);

    glBindVertexArray(jointArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, jointBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis), axis, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(2.0f);
}

void Character::render(Shader& shader, float deltaTime)
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    Bone* root = _skeleton->getRoot();
    if (_blendWeight < BLEND_TIME)
        _blendWeight += deltaTime;
    _motionList[static_cast<int32>(_currentState)]->updateKeyFrameTime(deltaTime);
    drawBone(root, modelMatrix, shader);
}

void Character::drawBone(Bone* bone, glm::mat4 matrix, Shader& shader)
{
    glm::quat blendBoneAnimationData = _motionList[static_cast<int32>(_currentState)]->getBoneAnimation(bone->index);
    if (_blendWeight < BLEND_TIME)
    {
        glm::quat prevBoneAnimationData = _motionList[static_cast<int32>(_prevState)]->getBoneAnimation(bone->index);
        blendBoneAnimationData = glm::slerp(prevBoneAnimationData, blendBoneAnimationData, _blendWeight / BLEND_TIME);
    }

    glm::mat4 model = matrix * bone->toParent * glm::mat4(blendBoneAnimationData);

    glm::vec3 direction = glm::vec3(-bone->direction.x, -bone->direction.y, -bone->direction.z);
    glm::vec3 rotAxis = glm::cross({ 1.0f, 0.0f, 0.0f }, direction);
    float angle = acos(glm::dot({ 1.0f, 0.0f, 0.0f }, glm::normalize(direction)));
    glm::mat4 boneRotation = glm::rotate(glm::mat4(1.0f), angle, rotAxis);
   
    float scale = glm::length(direction);
    glm::mat4 scaler = glm::scale(glm::mat4(1.0f), { scale, 1.0f, 1.0f });

    glBindVertexArray(boneArrayObject);
    shader.setUniformMat4("model", model * boneRotation * scaler);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(jointArrayObject);
    shader.setUniformMat4("model", model);
    glDrawArrays(GL_LINES, 0, 12);

    for (Bone* child : bone->childList)
        drawBone(child, model, shader);
}

void Character::setCharacterState(CharacterState state)
{
    _motionList[static_cast<int32>(_prevState)]->resetKeyFrameTime();

    _prevState = _currentState;
    _currentState = state;

    _blendWeight = BLEND_TIME - _blendWeight;
}

void Character::move()
{

}

void Character::rotate()
{

}