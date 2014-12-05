
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
    appfuncs->scrollEvent(y);
}

int main()
{
    glfwSetErrorCallback(handleGlfwErr);
    if (!glfwInit())
        Log::fatalError("glfwInit failed.");
    atexit(glfwTerminate);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

    // get the best video mode
    int modeCount;
    const GLFWvidmode *modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &modeCount);
    int windowWidth = modes[modeCount - 1].width;
    int windowHeight = modes[modeCount - 1].height;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Blocks", glfwGetPrimaryMonitor(), NULL);
    if (!window)
        Log::fatalError("glfwCreateWindow failed.");
    glfwSetKeyCallback(window, handleGlfwKey);
    glfwSetMouseButtonCallback(window, handleGlfwClick);
    glfwSetCursorPosCallback(window, handleGlfwMouseMove);
    glfwSetScrollCallback(window, handleGlfwScroll);
    glfwMakeContextCurrent(window);

    Log::log((char const*) glGetString(GL_VERSION));

    appfuncs = new AppFuncs(window, float(windowWidth) / windowHeight);

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
