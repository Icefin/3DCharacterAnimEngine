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

    if (_motion != NULL)
        delete _motion;
}

void	Character::initialize(Skeleton* skeleton, Motion* motion)
{
	_skeleton = skeleton;
	_motion = motion;

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(joint), joint, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(2.0f);
}

void    Character::update(Shader& shader, float time)
{
    Bone* root = _skeleton->getRoot();
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    drawBone(root, modelMatrix, shader, time);
}

void    Character::drawBone(Bone* bone, glm::mat4 matrix, Shader& shader, float time)
{
    Posture* motionData = _motion->getBonePostureAtFrame(bone->index, floor(time));
    glm::quat qmotion = dequantizeQuaternion(motionData->qrotation);

    //glm::mat4 model = matrix * bone->toParent /* motionData->rotation*/;
    glm::mat4 model = matrix * bone->toParent * glm::mat4(qmotion);

    glm::vec3 direction = glm::vec3(-bone->direction.x, -bone->direction.y, -bone->direction.z);
    glm::vec3 rotAxis = glm::cross({ 1.0f, 0.0f, 0.0f }, direction);
    float angle = acos(glm::dot(glm::normalize(direction), { 1.0f, 0.0f, 0.0f }));
    glm::mat4 boneRotation = glm::rotate(glm::mat4(1.0f), angle, rotAxis);

    float scale = glm::length(direction);
    glm::mat4 scaler = glm::scale(glm::mat4(1.0f), { scale, 1.0f, 1.0f });

    glBindVertexArray(boneArrayObject);
    shader.setUniformMat4("model", model * boneRotation * scaler);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(jointArrayObject);
    shader.setUniformMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    for (Bone* child : bone->childList)
        drawBone(child, model, shader, floor(time));
}