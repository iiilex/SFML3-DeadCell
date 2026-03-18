#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

#include <Game.h>

Game::Game() : 
window(sf::VideoMode({1920, 1080}), "Dead Cell", sf::Style::Titlebar | sf::Style::Close),
player(),
system()
{
    window.setFramerateLimit(120);
}

void Game::run()
{
    std::cout << "Run!";

    sf::Clock clock;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (event->is<sf::Event::KeyPressed>()) 
            {
                if (auto keyEvent = event->getIf<sf::Event::KeyPressed>())
                {
                    if (keyEvent->code == sf::Keyboard::Key::Escape)
                        window.close();
                    if (keyEvent->code == sf::Keyboard::Key::Space || keyEvent->code == sf::Keyboard::Key::W)
                        player.jump();
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            player.runLeft();
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            player.runRight();
        else  
            player.endRun();


        sf::Time deltaTime = clock.restart();
        float dt = deltaTime.asSeconds();
        if(dt>0.05)  dt=0.05;

        player.update(window, system, dt);
        
        player.draw(window);
    }
}