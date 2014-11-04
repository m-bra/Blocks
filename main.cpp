
#include "config.h"
#include "AppFuncs.hpp"
#include <SFML/Window.hpp>

sf::Window window(sf::VideoMode::getDesktopMode(), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
AppFuncs appFuncs(&window);
bool running = true;

int main()
{
    window.setVerticalSyncEnabled(true);

	sf::Clock clock;
    while (appFuncs.running())
    {

		sf::Event event;
		while (window.pollEvent(event))
		{
			switch(event.type)
			{
			case sf::Event::KeyPressed:
			{
				unsigned mod = 0;
				if (event.key.control)
					mod|= 1 << AppFuncs::CONTROL_BIT;
				if (event.key.shift)
					mod|= 1 << AppFuncs::SHIFT_BIT;
				if (event.key.alt)
					mod|= 1 << AppFuncs::ALT_BIT;
				if (event.key.system)
					mod|= 1 << AppFuncs::SYSTEM_BIT;
				appFuncs.keyEvent(event.key.code, true, mod);
				break;
			}
			case sf::Event::KeyReleased:
			{
				unsigned mod = 0;
				if (event.key.control)
					mod|= 1 << AppFuncs::CONTROL_BIT;
				if (event.key.shift)
					mod|= 1 << AppFuncs::SHIFT_BIT;
				if (event.key.alt)
					mod|= 1 << AppFuncs::ALT_BIT;
				if (event.key.system)
					mod|= 1 << AppFuncs::SYSTEM_BIT;
				appFuncs.keyEvent(event.key.code, false, mod);
				break;
			}
			case sf::Event::MouseButtonPressed:
				appFuncs.clickEvent(event.mouseButton.button, true, event.mouseButton.x, event.mouseButton.y);
				break;
			case sf::Event::MouseButtonReleased:
				appFuncs.clickEvent(event.mouseButton.button, false, event.mouseButton.x, event.mouseButton.y);
				break;
			case sf::Event::MouseMoved:
				appFuncs.moveEvent(event.mouseMove.x, event.mouseMove.y);
				break;
			case sf::Event::MouseWheelMoved:
				appFuncs.scrollEvent(event.mouseWheel.delta);
				break;
			case sf::Event::Closed:
				running = false;
				break;
			case sf::Event::Resized:
				glViewport(0, 0, event.size.width, event.size.height);
				break;
			default:
				break;
			}
		}

		float time = clock.restart().asSeconds();

		appFuncs.update(time);
		appFuncs.render();

		window.display();
    }

    return 0;
}
