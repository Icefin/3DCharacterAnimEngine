#include "Plane.h"

Plane::Plane(glm::vec3 position)
{
	_position = position;

    glGenBuffers(1, &_vbo);
    glGenVertexArrays(1, &_vao);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // position attribute
    //lcoation = 0, sizeof attrib(vec3), type, want to be normalized?, stride(space btw vertattb.), offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

Plane::~Plane(void)
{

}

void Plane::render(Shader& shader)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(_vao);
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), _position);
    shader.setUniformMat4("model", modelMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}