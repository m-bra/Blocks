#include "precompiled.hpp"

#include "AppFuncs.hpp"

namespace blocks
{

AppFuncs::AppFuncs(GLFWwindow *window)
    : window(window)
{
    std::vector<Module *> modules;
    physics::BulletPhysics *physics = new physics::BulletPhysics;
    graphics::DefaultGraphics *graphics = new graphics::DefaultGraphics;
    modules.push_back(new logic::DefaultLogic);
    modules.push_back(physics);
    modules.push_back(graphics);
    world = new World(modules, graphics, physics);
    updateMouseGrab();
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    glfwSetCursorPos(window, w/2, h/2);
    thread = std::thread(&AppFuncs::parallel, this);
}

AppFuncs::~AppFuncs()
{
    delete world;
    _running = false;
    thread.join();
}

void AppFuncs::parallel()
{
    Log::setThisThreadName("Parallel");

    double lastTime;
    while (_running)
    {
        double currentTime = glfwGetTime();
        double time = currentTime - lastTime;
        world->parallel(time);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        lastTime = currentTime;
    }
}

void AppFuncs::keyEvent(int key, int action, int mods)
{
    if (action == GLFW_PRESS)
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
        {
            mouseDetached = !mouseDetached;
            // center the mouse
            ivec3 center;
            glfwGetWindowSize(window, &center.x, &center.y);
            center/= 2;
            glfwSetCursorPos(window, center.x, center.y);
            updateMouseGrab();
            break;
        }
        case GLFW_KEY_ENTER:
            world->resetPlayer(world->playerEntity);
            break;
        case GLFW_KEY_SPACE:
            world->jump(world->playerEntity);
            break;
        default:;
        }

    switch (key)
    {
    case GLFW_KEY_W:
    case GLFW_KEY_UP:
        moveForward = action != GLFW_RELEASE;
        break;
    case GLFW_KEY_S:
    case GLFW_KEY_DOWN:
        moveBackward = action != GLFW_RELEASE;
        break;
    case GLFW_KEY_A:
    case GLFW_KEY_LEFT:
        moveLeft = action != GLFW_RELEASE;
        break;
    case GLFW_KEY_D:
    case GLFW_KEY_RIGHT:
        moveRight = action != GLFW_RELEASE;
        break;
    }
}

void AppFuncs::clickEvent(int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (mouseDetached)
        {
            mouseDetached = false;
            updateMouseGrab();
            // center the mouse
            ivec3 center;
            glfwGetWindowSize(window, &center.x, &center.y);
            center/= 2;
            glfwSetCursorPos(window, center.x, center.y);
        }
        else
        {
            if (button == GLFW_MOUSE_BUTTON_LEFT)
                world->take(world->playerEntity, 0);
            else if (button == GLFW_MOUSE_BUTTON_RIGHT)
                world->place(world->playerEntity, 0);
        }
    }
}

void AppFuncs::scrollEvent(double ticks)
{
	float &holdDis = world->entityHoldDistances[world->playerEntity][0];
	holdDis+= ticks * .5;
	holdDis = glm::clamp(holdDis, 2.f, world->reach-1.f);
}

void AppFuncs::update(float time)
{
    ivec3 mouseDiff = ivec3(0, 0, 0);

    if (!mouseDetached)
    {
        ivec3 center;
        glfwGetWindowSize(window, &center.x, &center.y);
        center/= 2;

        dvec3 mousePos;
        glfwGetCursorPos(window, &mousePos.x, &mousePos.y);

        mouseDiff = center - mousePos;
        glfwSetCursorPos(window, center.x, center.y);
    }

    rotation.x+= mouseDiff.x * time * .2;
    rotation.y+= mouseDiff.y * time * .2;

    glm::vec4 const camDir4 = glm::vec4(0, 0, -1, 0);
    glm::vec4 const camLeft4 = glm::vec4(1, 0, 0, 0);
    glm::vec4 const camUp4 = glm::vec4(0, 1, 0, 0);

    glm::vec3 m;
    if (moveForward) m.z+= 1;
    if (moveBackward) m.z-= 1;
    if (moveRight) m.x+= 1;
    if (moveLeft) m.x-= 1;
    world->setWalk(world->playerEntity, m);

    glm::mat4 mat = glm::yawPitchRoll(rotation.x, rotation.y, rotation.z);
    world->graphics->camDir = glm::vec3(mat * camDir4);
    world->graphics->camLeft = glm::vec3(mat * camLeft4);
    world->graphics->camUp = glm::vec3(mat * camUp4);

    world->update(time);

    if (world->loading)
        glfwSetWindowTitle(window, "Loading");
    else
        glfwSetWindowTitle(window, "Blocks");
}

}
