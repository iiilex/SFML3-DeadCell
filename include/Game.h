#pragma once

#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>

#include<Animation.h>
#include<Player.h>
#include<Physics.h>

class Game
{
    protected:
        sf::RenderWindow window;
        Player player;
        PhysicsSystem system;
    public:
        Game();
        void run();
};
