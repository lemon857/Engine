
#include <glm/vec2.hpp>

#include <iostream>
#include <chrono>

#include "Resources/ResourceManager.h"
#include "Game/Game.h"


glm::ivec2 g_WindowSize(640, 480);

Game g_Game(g_WindowSize);

void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height) 
{
    g_WindowSize.x = width;
    g_WindowSize.y = height;
    glViewport(0, 0, width, height);
}

void glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mode) 
{
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(pWindow, GL_TRUE);

    }
    g_Game.setKey(key, action);
}

int main(int argc, char** argv)
{
    if (!glfwInit())
    {
        std::cout << "glfwInit failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* pWindow = glfwCreateWindow(g_WindowSize.x, g_WindowSize.y, "Test Game", nullptr, nullptr);

    if (!pWindow)
    {

        std::cout << "glfwGenerateWindow failed\n";
        glfwTerminate();
        return -1;
    }
    glfwSetWindowSizeCallback(pWindow, glfwWindowSizeCallback);
    glfwSetKeyCallback(pWindow, glfwKeyCallback);
    glfwMakeContextCurrent(pWindow);

	if (!gladLoadGL())
	{
		std::cout << "gladLoadGL failed\n";
		return -1;
	}
	
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";

    ResourceManager::setExecutablePath(argv[0]);
	
    if (!g_Game.init())
    {
        std::cout << "Game init failed\n";
        return -1;
    }

	glClearColor(0, 0, 0, 1);   
   
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(pWindow))
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        uint64_t duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - lastTime).count();
        lastTime = currentTime;

        g_Game.update(duration);

        glClear(GL_COLOR_BUFFER_BIT);

        g_Game.render();

        glfwSwapBuffers(pWindow);

        glfwPollEvents();
    }
    ResourceManager::unloadAllResources();
    glfwTerminate();
    return 0;
}