// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#ifndef CAMERA3D_H
#define CAMERA3D_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <iostream>

class Camera3D
{
public:
    Camera3D(uint32 screenWidth, uint32 screenHeight, glm::vec3* targetPosition) : _mouseSensitivity(0.2f), _zoom(45.0f)
    {
        _screenWidth = screenWidth;
        _screenHeight = screenHeight;

        _targetPosition = targetPosition;
        _cameraPosition = glm::vec3(_radius, 0.0f, 0.0f);
        _radius = 60.0f;
        _azimuth = 0.0f;
        _elevation = 0.0f;

        prevCursorX = _screenWidth / 2;
        prevCursorY = _screenHeight / 2;

    }

    void update(Shader& shader)
    {
        calculateCameraPosition();

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(_zoom), (float)_screenWidth / (float)_screenHeight, 0.1f, 1000.0f);
        shader.setUniformMat4("projection", projectionMatrix);

        glm::mat4 viewMatrix = glm::lookAt(_cameraPosition + *_targetPosition, *_targetPosition, glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setUniformMat4("view", viewMatrix);
    }

public :
    float       prevCursorX;
    float       prevCursorY;

    void processMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= _mouseSensitivity;
        yoffset *= _mouseSensitivity;

        _azimuth += xoffset;
        _elevation += yoffset;

        if (_elevation > 89.0f)
            _elevation = 89.0f;
        if (_elevation < -89.0f)
            _elevation = -89.0f;

        if (_azimuth > 360.0f)
            _azimuth = 0.0f;
        if (_azimuth < 0.0f)
            _azimuth = 360.0f;
    }

    void processMouseScroll(float yoffset)
    {
        _zoom -= static_cast<float>(yoffset);
        if (_zoom < 1.0f)
            _zoom = 1.0f;
        if (_zoom > 100.0f)
            _zoom = 100.0f;
    }

private:
    uint32      _screenWidth;
    uint32      _screenHeight;

    glm::vec3*   _targetPosition;
    glm::vec3   _cameraPosition;
    float       _radius;
    float       _azimuth;
    float       _elevation;

    float       _mouseSensitivity;
    float       _zoom;

    void        calculateCameraPosition(void) 
    {
        float unitX = glm::cos(glm::radians(_elevation)) * glm::cos(glm::radians(_azimuth));
        float unitY = glm::sin(glm::radians(_elevation));
        float unitZ = glm::cos(glm::radians(_elevation)) * glm::sin(glm::radians(_azimuth));
        
        _cameraPosition = _radius * glm::vec3(unitX, unitY, unitZ);
    }
};
#endif