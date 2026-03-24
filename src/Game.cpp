#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <optional>

#include <Game.h>

Game::Game() : 
window(sf::VideoMode({1920, 1080}), "Dead Cell", sf::Style::Titlebar | sf::Style::Close)
{
    window.setFramerateLimit(120);
}

void Game::run()
{
    std::cout << "Run!";

    sf::Clock clock;

    std::optional<Player> player;
    player.emplace();

    PhysicsSystem mySystem;

    clock.reset();
    while (window.isOpen())
    {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D))
        {
            player->rightPressed(true);
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A))
        {
            player->leftPressed(true);
        }
        else
        {
            player->leftPressed(false);
            player->leftPressed(false);
        }
        

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S))
        {
            player->crouchPressed(true);
        }
        else
        {
            player->crouchPressed(false);
        }

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) 
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();

                if (keyPressed->scancode == sf::Keyboard::Scancode::Up || keyPressed->scancode == sf::Keyboard::Scancode::Space)
                    player->jump();

                if (keyPressed->scancode == sf::Keyboard::Scancode::J)
                    player->roll();

                if (keyPressed->scancode == sf::Keyboard::Scancode::K)
                    player->attack();

                if(keyPressed->scancode == sf::Keyboard::Scancode::L)
                    player->counter();
            }
        }

        float dt = clock.restart().asSeconds();
        player->updatePlayer(dt, mySystem, window);
        

    }
    
}