#include <queue>
#include "Character.h"
#include "Transform.h"

unsigned int vertexBufferObject, vertexArrayObject;

Character::~Character()
{
    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);
}

void	Character::initialize(Skeleton* skeleton, Motion* motion)
{
	_skeleton = skeleton;
	_motion = motion;

    //vbo : object vertex set
    //vao : attribute of vertex set
    //ebo : vertex index order of object

    glGenBuffers(1, &vertexBufferObject);
    glGenVertexArrays(1, &vertexArrayObject);

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW); //copy user-defined data into buffer

    // position attribute
    //lcoation = 0, sizeof attrib(vec3), type, want to be normalized?, stride(space btw vertattb.), offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void    Character::draw(Shader& shader, int32 frame)
{
    Bone* root = _skeleton->getRoot();
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    drawBone(root, modelMatrix, shader);
}

void    Character::drawBone(Bone* bone, glm::mat4 matrix, Shader& shader)
{
    if (bone == NULL)
        return;
    
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), bone->translation);

    glm::quat toParent = dequantizeQuaternion(bone->toParent, QUANT_SCALE);
    glm::mat4 rotation = glm::toMat4(toParent);

    //motion...

    glm::mat4 model = matrix * rotation * translation;
    shader.setUniformMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    for (Bone* child : bone->childList)
        drawBone(child, model, shader);
}