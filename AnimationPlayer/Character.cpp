// author: Geonho Shin (icefin@pearlabyss.com)
#include <queue>

#include "Character.h"
#include "CustomMath.h"

uint32 axisBufferObject, axisArrayObject;

Character::Character(Skeleton* skeleton, std::vector<Motion*>& motionList)
{
    _skeleton = skeleton;
    _animator = new Animator(motionList);
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
    glDeleteVertexArrays(1, &axisArrayObject);
    glDeleteBuffers(1, &axisBufferObject);

    if (_skeleton != NULL)
        delete _skeleton;

    if (_animator != NULL)
        delete _animator;
}

void Character::render(Shader& shader, float deltaTime)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(2.0f);

    _animator->update(_currentState, deltaTime);
    updateMatrixPalette();
    renderSkeleton(shader);
}

void Character::render(Shader& shader)
{
    __noop;
}

void Character::updateMatrixPalette()
{
    _matrixPalette[0] = glm::mat4(_animator->getJointAnimation(0));

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

        glGenBuffers(1, &boneBufferObject);
        glGenVertexArrays(1, &boneArrayObject);

        glBindVertexArray(boneArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, boneBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(boneArrayObject);
        shader.setUniformMat4("model", glm::mat4(1.0f));
        glLineWidth(8.0f);
        glDrawArrays(GL_LINES, 0, 6);
        
        glDeleteVertexArrays(1, &boneArrayObject);
        glDeleteBuffers(1, &boneBufferObject);

        glBindVertexArray(axisArrayObject);
        shader.setUniformMat4("model", _matrixPalette[index]);
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, 12);
    }
}

void Character::setCharacterState(AnimationState state)
{
    _currentState = state;
}

void Character::move()
{

}

void Character::rotate()
{

}
