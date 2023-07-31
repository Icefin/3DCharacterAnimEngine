// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#ifndef CAMERA3D_H
#define CAMERA3D_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class CameraDirection 
{
    Forward = 0,
    Backward,
    Left,
    Right
};

// Default camera values
constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 2.5f;
constexpr float SENSITIVITY = 0.01f;
constexpr float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera3D
{
public:
    Camera3D(uint32 screenWidth, uint32 screenHeight, glm::vec3 targetPosition = glm::vec3(0.0f, 0.0f, 0.0f)) : _mouseSensitivity(0.01f), _zoom(45.0f)
    {
        _screenWidth = screenWidth;
        _screenHeight = screenHeight;

        _targetPosition = targetPosition;
        _cameraPosition = glm::vec3(-_radius, 0.0f, 0.0f);
        _radius = 60.0f;
        _azimuth = 0.0f;
        _elevation = 0.0f;
    }
    /*
    // constructor with vectors
    Camera3D(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera3D(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    */
    void update(Shader& shader, float deltaTime)
    {
        calculateCameraPosition();

        glm::mat4 projection = glm::perspective(glm::radians(_zoom), (float)_screenWidth / (float)_screenHeight, 0.1f, 100.0f);
        shader.setUniformMat4("projection", projection);

        glm::mat4 view = getViewMatrix();
        shader.setUniformMat4("view", view);
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix(void)
    {
        return glm::lookAt(_cameraPosition, _targetPosition, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= _mouseSensitivity;
        yoffset *= _mouseSensitivity;

        _azimuth += xoffset;
        _elevation += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (_elevation > 89.0f)
                _elevation = 89.0f;
            if (_elevation < -89.0f)
                _elevation = -89.0f;

            if (_azimuth > 359.0f)
                _azimuth = 0.0f;
            if (_azimuth < 0.0f)
                _azimuth = 359.0f;
        }
        // update Front, Right and Up Vectors using the updated Euler angles
        //updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void processMouseScroll(float yoffset)
    {
        _zoom -= static_cast<float>(yoffset);
        if (_zoom < 1.0f)
            _zoom = 1.0f;
        if (_zoom > 80.0f)
            _zoom = 80.0f;
    }

private:
    uint32      _screenWidth;
    uint32      _screenHeight;

    glm::vec3   _targetPosition;
    glm::vec3   _cameraPosition;
    float       _radius;
    float       _azimuth;
    float       _elevation;

    float       _mouseSensitivity;
    float       _zoom;

    void        calculateCameraPosition(void) 
    {
        float unitX = glm::cos(_elevation) * glm::cos(_azimuth);
        float unitY = glm::sin(_elevation);
        float unitZ = glm::cos(_elevation) * glm::sin(_azimuth);
        
        _cameraPosition = _radius * glm::vec3(unitX, unitY, unitZ);
    }
    /*
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors(void)
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(_azimuth)) * cos(glm::radians(_elevation));
        front.y = sin(glm::radians(_elevation));
        front.z = sin(glm::radians(_azimuth)) * cos(glm::radians(_elevation));
        _frontVector = glm::normalize(front);
        // also re-calculate the Right and Up vector
        _rightVector = glm::normalize(glm::cross(_frontVector, _worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        _upVector = glm::normalize(glm::cross(_rightVector, _frontVector));
    }
    */
};
#endif