#include <SFML/Graphics.hpp>
#include "Game.h"
#include "Renderer.h"

int main()
{
	sf::RenderWindow window(sf::VideoMode(1200, 900), "BETTER Mario");
	sf::Clock deltaClock;
	Renderer renderer(window);
	window.setFramerateLimit(60);

	Begin();
	while (window.isOpen())
	{
		float deltaTime = deltaClock.restart().asSeconds();

		sf::Event event{};
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::Escape)
				paused = !paused;
		}

		Update(deltaTime);

		window.clear();

		window.setView(camera.GetView(window.getSize()));
		Render(renderer);

		window.setView(camera.GetUIView());
		RenderUI(renderer);

		window.display();
	}
}
