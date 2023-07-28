// author: Geonho Shin (icefin@pearlabyss.com)
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>

#include <iostream>

#include "Shader.h"
#include "Camera3D.h"
#include "CharacterLoader.h"
#include "Plane.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

constexpr uint32 SCR_WIDTH = 800;
constexpr uint32 SCR_HEIGHT = 600;
constexpr uint32 FRAME_RATE = 120;
// character
CharacterLoader characterLoader;
Character* character;

// camera
Camera3D camera(glm::vec3(0.0f, 0.0f, 50.0f));
float prevX = SCR_WIDTH / 2.0f;
float prevY = SCR_HEIGHT / 2.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLFWwindow* window;

void framebuffer_size_callback(GLFWwindow* window, int32 width, int32 height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void loadCharacter()
{
    std::string skeleton = "./test/skeleton.asf";
    std::vector<std::string> motions = { "./test/idle.amc",
                                            "./test/forward.amc",
                                            "./test/backward.amc",
                                            "./test/run.amc",
                                            "./test/jump.amc",
                                            "./test/attack.amc"
                                        };
    //skin
  
    character = characterLoader.loadCharacter(skeleton, motions);
}

void initializeGLContext(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Animation Player", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << '\n';
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false)
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    glEnable(GL_DEPTH_TEST);
}

int main()
{
    initializeGLContext();

    Shader shader("./shaders/vertexShader.vert", "./shaders/fragmentShader.frag");
    shader.use();

    Plane* ground = new Plane(glm::vec3(0.0f, -18.0f, 0.0f));
    loadCharacter();

    while (glfwWindowShouldClose(window) == false)
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setUniformMat4("projection", projection);

        glm::mat4 view = camera.getViewMatrix();
        shader.setUniformMat4("view", view);

        ground->render(shader);
        character->render(shader, deltaTime * FRAME_RATE);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_FALSE)
    {
        if (character->getCharacterState() != AnimationState::FORWARD)
            character->setCharacterState(AnimationState::FORWARD);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        if (character->getCharacterState() != AnimationState::RUN)
            character->setCharacterState(AnimationState::RUN);
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (character->getCharacterState() != AnimationState::BACKWARD)
            character->setCharacterState(AnimationState::BACKWARD);
    }
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (character->getCharacterState() != AnimationState::JUMP)
            character->setCharacterState(AnimationState::JUMP);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (character->getCharacterState() != AnimationState::ATTACK)
            character->setCharacterState(AnimationState::ATTACK);
    }
    else
    {
        if (character->getCharacterState() != AnimationState::IDLE)
            character->setCharacterState(AnimationState::IDLE);
    }
/*
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CameraDirection::Left, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CameraDirection::Right, deltaTime);
*/
}

void framebuffer_size_callback(GLFWwindow* window, int32 width, int32 height)
{
    // make sure the viewport matches the new window dimensions
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    float xoffset = xpos - prevX;
    // reversed since y-coordinates go from bottom to top
    float yoffset = prevY - ypos;

    prevX = xpos;
    prevY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(static_cast<float>(yoffset));
}