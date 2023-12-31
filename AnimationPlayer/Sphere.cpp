#include <vector>

#include "Sphere.h"

Sphere::Sphere(glm::vec3 position, float radius, glm::vec3 color, uint32 stackNumber, uint32 sectorNumber)
{
    _position = position;

    std::vector<float> vertices;
    static constexpr float kPI = glm::pi<float>();
    float sectorStep = 2 * kPI / sectorNumber;
    float stackStep = kPI / stackNumber;

    float sectorAngle, stackAngle;
    float x, y, z, xy;

    for (int32 i = 0; i <= stackNumber; ++i)
    {
        stackAngle = kPI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for (int32 j = 0; j <= sectorNumber; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(color.x);
            vertices.push_back(color.y);
            vertices.push_back(color.z);
        }
    }

    std::vector<uint32> indices;
    int32 k1, k2;
    for (int32 i = 0; i < stackNumber; ++i)
    {
        k1 = i * (sectorNumber + 1);     // beginning of current stack
        k2 = k1 + sectorNumber + 1;      // beginning of next stack

        for (int32 j = 0; j < sectorNumber; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackNumber - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    _indicesNumber = indices.size();

    glGenBuffers(1, &_vbo);
    glGenVertexArrays(1, &_vao);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Sphere::~Sphere(void)
{
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ebo);
}

void Sphere::update(float deltaTime)
{
    __noop;
}

void Sphere::render(Shader& shader)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

    glm::mat4 worldMat = glm::translate(glm::mat4(1.0f), _position);
    shader.setUniformMat4("worldMat", worldMat);
    glDrawElements(GL_TRIANGLES, _indicesNumber, GL_UNSIGNED_INT, NULL);
}