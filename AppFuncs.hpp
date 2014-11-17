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

#include <SFML/Window.hpp>

#include "vec.hpp"

#include "Shared.hpp"
#include "logic/Module.hpp"
#include "graphics/Module.hpp"
#include "physics/Module.hpp"

class AppFuncs
{
private:
	sf::Window *window;
	sf::Vector2i const windowSize;
	sf::Vector2i const center;
	class int = 0;

	Shared shared;
	bool db_pause = false, _running = true;
	glm::vec3 rotation;

	std::thread thread;
public:
	enum ModBits {CONTROL_BIT, SHIFT_BIT, ALT_BIT, SYSTEM_BIT};

	AppFuncs(sf::Window *window)
		: window(window), windowSize(sf::Vector2i(window->getSize())), center(windowSize / 2),
		  shared((float)windowSize.x / windowSize.y), thread(&AppFuncs::parallel, this)
	{
		sf::Mouse::setPosition(center, *window);
		window->setKeyRepeatEnabled(false);
	}

	~AppFuncs()
	{
		thread.join();
	}

	void parallel()
	{
#ifdef NOT_PARALLEL
#error NOPE
#endif
		sf::Clock clock;
		while (_running)
		{
			Time time = clock.restart().asSeconds();
			shared.logic.parallel(time);
			shared.physics.parallel(time);
			shared.graphics.parallel(time);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	bool running()
	{
		return _running;
	}

	void keyEvent(sf::Keyboard::Key key, bool pressed, unsigned mods)
	{
		if (pressed)
			switch (key)
			{
			case sf::Keyboard::Escape:
				_running = false;
				break;
			case sf::Keyboard::P:
				db_pause = !db_pause;
				break;
			case sf::Keyboard::Return:
				shared.logic.resetPlayer();
				break;
			case sf::Keyboard::Space:
				shared.logic.jump();
				break;
			case sf::Keyboard::E:
			{
				int e = shared.createEntity(EntityType::BLOCK,
								shared.camPos + shared.camDir * (shared.logic.holdDistance + 1));
							shared.logic.entityLogics[e].blockEntity.blockType = BlockType::COMPANION;
			}
				break;
			default:;
			}
	}

	void clickEvent(sf::Mouse::Button button, bool pressed, int x, int y)
	{
		if (pressed)
		{
			if (button == sf::Mouse::Left)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
					shared.logic.supertake();
				else
					shared.logic.take();
			}
			else if (button == sf::Mouse::Right)
				shared.logic.place();
		}
	}

	void scrollEvent(int ticks)
	{
		float &holdDis = shared.logic.holdDistance;
		holdDis+= ticks * .5;
		holdDis = glm::clamp(holdDis, 2.f, shared.logic.reach-1.f);
	}

	void moveEvent(int x, int y)
	{

	}

	void update(float time)
	{
		sf::Vector2i mouseDiff = sf::Vector2i(0, 0);
		if (!db_pause)
		{
			mouseDiff = sf::Vector2i(center - sf::Mouse::getPosition(*window));
			sf::Mouse::setPosition(center, *window);
		}

		rotation.x+= mouseDiff.x * time * .2;
		rotation.y+= mouseDiff.y * time * .2;

		glm::vec4 const camDir4 = glm::vec4(0, 0, -1, 0);
		glm::vec4 const camLeft4 = glm::vec4(1, 0, 0, 0);
		glm::vec4 const camUp4 = glm::vec4(0, 1, 0, 0);

		glm::vec3 m;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			m.z+= 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			m.z-= 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			m.x+= 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			m.x-= 1;
		shared.logic.setWalk(m);

		glm::mat4 mat = glm::yawPitchRoll(rotation.x, rotation.y, rotation.z);
		shared.camDir = glm::vec3(mat * camDir4);
		shared.camLeft = glm::vec3(mat * camLeft4);
		shared.camUp = glm::vec3(mat * camUp4);

		Time worldTime = shared.secondsToWorldTime(time);
		shared.update(worldTime);

	}

	void render()
	{
		shared.graphics.render();
	}
};

#endif /* GAME_HPP_ */
