#include "Cube.h"

Cube::Cube(glm::vec3 position, glm::vec3 halfSideLength, glm::vec3 color)
{
    _position = position;

    float vertices[48] = {
        -halfSideLength.x, -halfSideLength.y, -halfSideLength.z, color.x, color.y, color.z,
        -halfSideLength.x, -halfSideLength.y, halfSideLength.z, color.x, color.y, color.z,
        halfSideLength.x, -halfSideLength.y, halfSideLength.z, color.x, color.y, color.z,
        halfSideLength.x, -halfSideLength.y, -halfSideLength.z, color.x, color.y, color.z,

        -halfSideLength.x, halfSideLength.y, -halfSideLength.z, color.x, color.y, color.z,
        -halfSideLength.x, halfSideLength.y, halfSideLength.z, color.x, color.y, color.z,
        halfSideLength.x, halfSideLength.y, halfSideLength.z, color.x, color.y, color.z,
        halfSideLength.x, halfSideLength.y, -halfSideLength.z, color.x, color.y, color.z
    };

    uint32 indices[36] = {
         1,0,2,
         2,0,3,
         1,2,5,
         2,6,5,
         3,7,6,
         3,6,2,
         6,4,5,
         6,7,4,
         1,5,4,
         1,4,0,
         0,4,7,
         3,0,7
    };

    glGenBuffers(1, &_vbo);
    glGenVertexArrays(1, &_vao);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Cube::~Cube(void)
{
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
}

void Cube::update(float deltaTime)
{
    __noop;
}

void Cube::render(Shader& shader)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

    glm::mat4 worldMat = glm::translate(glm::mat4(1.0f), _position);
    shader.setUniformMat4("worldMat", worldMat);
    glDrawElements(GL_LINES, 36, GL_UNSIGNED_INT, NULL);
}