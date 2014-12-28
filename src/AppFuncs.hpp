#ifndef GAME_HPP_
#define GAME_HPP_

#include <iostream>
#include <tuple>
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
	AppFuncs(GLFWwindow *window)
		: window(window)
	{
		logicModule = new logic::Module;
		graphicsModule = new graphics::Module;
		physicsModule = new physics::Module;
		world = new World(&std::array<Registerable *, 3>{logicModule, graphicsModule, physicsModule}[0], 3);
		updateMouseGrab();
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		glfwSetCursorPos(window, w/2, h/2);
		thread = std::thread(&AppFuncs::parallel, this);
	}

	~AppFuncs()
	{
		delete world;
		delete physicsModule;
		delete graphicsModule;
		delete logicModule;
		_running = false;
		thread.join();
	}

	void parallel()
	{
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

	bool running()
	{
		return _running;
	}

	void frameBufEvent(int x, int y)
	{
		world->setWindowSize(x, y);
	}

	void keyEvent(int key, int action, int mods)
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
				logicModule->resetPlayer();
				break;
			case GLFW_KEY_SPACE:
				logicModule->jump();
				break;
			case GLFW_KEY_E:
			{
				float const holdDistance = logicModule->entityLogics[world->playerEntity].player.holdDistance;
				fvec3 pos = fvec3(world->getEntityPos(world->playerEntity) + world->camDir * (holdDistance + 1));
				int e = world->createEntity(EntityArgs {{"type", (intptr_t) EntityType::BLOCK}, {"pos", (intptr_t) &pos}});

				logicModule->entityLogics[e].blockEntity.blockType = BlockType::COMPANION;
				break;
			}
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

	void clickEvent(int button, int action, int mods)
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
					logicModule->take();
				else if (button == GLFW_MOUSE_BUTTON_RIGHT)
					logicModule->place();
			}
		}
	}

	void scrollEvent(double ticks)
	{
		float &holdDis = logicModule->entityLogics[world->playerEntity].player.holdDistance;
		holdDis+= ticks * .5;
		holdDis = glm::clamp(holdDis, 2.f, world->reach-1.f);
	}

	void moveEvent(double x, double y)
	{

	}

	void update(float time)
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
		logicModule->setWalk(m);

		glm::mat4 mat = glm::yawPitchRoll(rotation.x, rotation.y, rotation.z);
		world->camDir = glm::vec3(mat * camDir4);
		world->camLeft = glm::vec3(mat * camLeft4);
		world->camUp = glm::vec3(mat * camUp4);

		world->update(time);

		if (world->loading)
			glfwSetWindowTitle(window, "Loading");
		else
			glfwSetWindowTitle(window, "Blocks");
	}

	void render()
	{
		graphicsModule->render();
	}
};

}

#endif /* GAME_HPP_ */
