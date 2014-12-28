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

void handleGlfwFrameBuf(GLFWwindow *window, int x, int y)
{
    Log::debug("handleGlfwFrameBuf() called.");
    appfuncs->frameBufEvent(x, y);
}

int main()
{
    glfwSetErrorCallback(handleGlfwErr);
    if (!glfwInit())
        Log::fatalError("glfwInit failed.");
    atexit(glfwTerminate);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

    bool const fullscreen = false;

    int windowWidth, windowHeight;

    if (fullscreen)
    {
        // get the best video mode
        int modeCount;
        GLFWvidmode const *modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &modeCount);
        windowWidth = modes[modeCount - 1].width;
        windowHeight = modes[modeCount - 1].height;
    }
    else
    {
        windowWidth = 800;
        windowHeight = 600;
    }
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Blocks", fullscreen ? glfwGetPrimaryMonitor() : 0, 0);
    if (!window)
        Log::fatalError("glfwCreateWindow failed.");
    glfwSetKeyCallback(window, handleGlfwKey);
    glfwSetMouseButtonCallback(window, handleGlfwClick);
    glfwSetCursorPosCallback(window, handleGlfwMouseMove);
    glfwSetScrollCallback(window, handleGlfwScroll);
    glfwSetFramebufferSizeCallback(window, handleGlfwFrameBuf);
    glfwMakeContextCurrent(window);

    Log::log((char const*) glGetString(GL_VERSION));

    appfuncs = new AppFuncs(window);
    appfuncs->frameBufEvent(windowWidth, windowHeight);

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
