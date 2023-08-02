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
#include "Sphere.h"
#include "Cube.h"
#include "PlaneCloth.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

constexpr uint32 SCR_WIDTH = 800;
constexpr uint32 SCR_HEIGHT = 600;
uint32 frameRate = 120;

// character
CharacterLoader characterLoader;
Character* character;

Camera3D* camera;
GLFWwindow* window;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow* window, float deltaTime);

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

    Cube* ground = new Cube(glm::vec3(0.0f, -18.0f, 0.0f), glm::vec3(100.0f, 0.2, 100.0f), glm::vec3(0.5f, 0.5f, 0.5f));
    Cube* cube = new Cube(glm::vec3(20.0f, -13.0f, 20.0f), glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    Sphere* sphere = new Sphere(glm::vec3(-15.0f, -13.0f, 15.0f), 5.0f);
    PlaneCloth* planeCloth = new PlaneCloth(glm::vec3(5.0f, 0.0f, 5.0f), 30, 30, 60, 60);
    loadCharacter();

    camera = new Camera3D(SCR_WIDTH, SCR_HEIGHT, &character->_position);

    std::vector<GameObject*> gameObjectList;
    gameObjectList.push_back(ground);
    gameObjectList.push_back(cube);
    gameObjectList.push_back(sphere);
    gameObjectList.push_back(character);
    gameObjectList.push_back(planeCloth);

    float lastFrame = 0.0f;
    while (glfwWindowShouldClose(window) == false)
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processInput(window, deltaTime * frameRate);

        camera->update(shader, deltaTime * frameRate);

        for (int32 idx = 0; idx < gameObjectList.size() - 1; ++idx)
            gameObjectList[idx]->update(shader, deltaTime * frameRate);
        gameObjectList.back()->update(shader, 0.003f);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();
    return 0;
}

void processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (character->getCharacterState() != AnimationState::IDLE)
        character->setCharacterState(AnimationState::IDLE);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_FALSE)
    {
        if (character->getCharacterState() != AnimationState::FORWARD)
            character->setCharacterState(AnimationState::FORWARD);
        character->move(true, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        if (character->getCharacterState() != AnimationState::RUN)
            character->setCharacterState(AnimationState::RUN);
        character->move(true, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (character->getCharacterState() != AnimationState::BACKWARD)
            character->setCharacterState(AnimationState::BACKWARD);
        character->move(false, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        character->rotate(true, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        character->rotate(false, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (character->getCharacterState() != AnimationState::JUMP)
            character->setCharacterState(AnimationState::JUMP);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (character->getCharacterState() != AnimationState::ATTACK)
            character->setCharacterState(AnimationState::ATTACK);
    }

    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
    {
        frameRate = frameRate == 300 ? frameRate : frameRate + 5;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
    {
        frameRate = frameRate == 10 ? frameRate : frameRate - 5;
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    float xoffset = xpos - camera->prevCursorX;
    // reversed since y-coordinates go from bottom to top
    float yoffset = camera->prevCursorY - ypos;

    camera->prevCursorX = xpos;
    camera->prevCursorY = ypos;

    camera->processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->processMouseScroll(static_cast<float>(yoffset));
}