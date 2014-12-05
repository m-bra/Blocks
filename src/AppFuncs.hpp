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

#include "Shared.hpp"
#include "logic/Module.hpp"
#include "graphics/Module.hpp"
#include "physics/Module.hpp"

namespace blocks
{

class AppFuncs
{
private:
	GLFWwindow *window;

	Shared shared;
	bool _running = true;
	glm::vec3 rotation;

	bool mouseDetached = false;
	bool moveForward, moveBackward, moveLeft, moveRight;

	std::thread thread;

	void updateMouseGrab()
	{
		glfwSetInputMode(window, GLFW_CURSOR, mouseDetached ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		//glfwSetInputMode(window, GLFW_CURSOR, mouseDetached ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_NORMAL);
	}
public:
	AppFuncs(GLFWwindow *window, float aspect)
		: window(window),
		  shared(aspect), thread(&AppFuncs::parallel, this)
	{
		updateMouseGrab();
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		glfwSetCursorPos(window, w/2, h/2);
	}

	~AppFuncs()
	{
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
			shared.logic.parallel(time);
			shared.physics.parallel(time);
			shared.graphics.parallel(time);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			lastTime = currentTime;
		}
	}

	bool running()
	{
		return _running;
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
				shared.logic.resetPlayer();
				break;
			case GLFW_KEY_SPACE:
				shared.logic.jump();
				break;
			case GLFW_KEY_E:
			{
				fvec3 pos = shared.camPos + shared.camDir * (shared.logic.holdDistance + 1);
				int e = shared.createEntity(EntityType::BLOCK, {"pos", &pos});
							shared.logic.entityLogics[e].blockEntity.blockType = BlockType::COMPANION;
			}
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
					shared.logic.take();
				else if (button == GLFW_MOUSE_BUTTON_RIGHT)
					shared.logic.place();
			}
		}
	}

	void scrollEvent(double ticks)
	{
		float &holdDis = shared.logic.holdDistance;
		holdDis+= ticks * .5;
		holdDis = glm::clamp(holdDis, 2.f, shared.reach-1.f);
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
		shared.logic.setWalk(m);

		glm::mat4 mat = glm::yawPitchRoll(rotation.x, rotation.y, rotation.z);
		shared.camDir = glm::vec3(mat * camDir4);
		shared.camLeft = glm::vec3(mat * camLeft4);
		shared.camUp = glm::vec3(mat * camUp4);

		shared.update(time);

		if (shared.loading)
			glfwSetWindowTitle(window, "Loading");
		else
			glfwSetWindowTitle(window, "Blocks");
	}

	void render()
	{
		shared.graphics.render();
	}
};

}

#endif /* GAME_HPP_ */
