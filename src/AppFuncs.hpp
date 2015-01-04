#ifndef APPFUNCS_HPP_INCLUDED
#define APPFUNCS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#endif

#include <thread>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "vec.hpp"

#include "World.hpp"
#include "logic/Module.hpp"
#include "graphics/Module.hpp"
#include "physics/Module.hpp"

namespace blocks
{

class AppFuncs
{
private:
	GLFWwindow *window;

	logic::Module *logicModule;
	graphics::Module *graphicsModule;
	physics::Module *physicsModule;
	World *world;
	bool _running = true;
	glm::vec3 rotation;

	bool mouseDetached = false;
	bool moveForward, moveBackward, moveLeft, moveRight;

	std::thread thread;

	void updateMouseGrab()
	{
		//glfwSetInputMode(window, GLFW_CURSOR, mouseDetached ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		glfwSetInputMode(window, GLFW_CURSOR, mouseDetached ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_NORMAL);
	}
public:
	AppFuncs(GLFWwindow *window);
	~AppFuncs();

	void parallel();
	bool running() {return _running;}

	void frameBufEvent(int x, int y) {world->setWindowSize(x, y);}
	void keyEvent(int key, int action, int mods);
	void clickEvent(int button, int action, int mods);
	void scrollEvent(double ticks);
	void moveEvent(double x, double y) {}

	void update(float time);
	void render() {graphicsModule->render();}
};

}

#endif /* GAME_HPP_ */
