
#include "config.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "AppFuncs.hpp"
#include "Logger.hpp"

using namespace blocks;

AppFuncs *appfuncs;

void handleGlfwErr(int error, const char *description)
{
    Log::error(description);
}

void handleGlfwKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    appfuncs->keyEvent(key, action, mods);
}

void handleGlfwClick(GLFWwindow *window, int key, int action, int mods)
{
    appfuncs->clickEvent(key, action, mods);
}

void handleGlfwMouseMove(GLFWwindow *window, double x, double y)
{
    appfuncs->moveEvent(x, y);
}

void handleGlfwScroll(GLFWwindow *window, double x, double y)
{
    appfuncs->scrollEvent(x);
}

int main()
{
    glfwSetErrorCallback(handleGlfwErr);
    if (!glfwInit())
        Log::fatalError("glfwInit failed.");
    atexit(glfwTerminate);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Blocks", glfwGetPrimaryMonitor(), NULL);
    if (!window)
        Log::fatalError("glfwCreateWindow failed.");
    glfwSetKeyCallback(window, handleGlfwKey);
    glfwSetMouseButtonCallback(window, handleGlfwClick);
    glfwSetCursorPosCallback(window, handleGlfwMouseMove);
    glfwSetScrollCallback(window, handleGlfwScroll);
    glfwMakeContextCurrent(window);

    Log::log((char const*) glGetString(GL_VERSION));

    appfuncs = new AppFuncs(window, 4./3.);

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window) && appfuncs->running())
    {
        double currentTime = glfwGetTime();
        double time = currentTime - lastTime;

        glfwPollEvents();
        appfuncs->update(time);

        appfuncs->render();
        glfwSwapBuffers(window);
        // currentTime = time of beginning of this frame
        lastTime = currentTime;
    }

    delete appfuncs;

    glfwDestroyWindow(window);
}
