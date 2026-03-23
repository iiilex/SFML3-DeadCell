#pragma once

#include<SFML/Graphics.hpp>

#include<Global.h>

struct PhysicsData
{
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::FloatRect box;
    PhysicsData();
};

class PhysicsSystem
{
    protected:
        const float gravity = 3500;
    public:
        PhysicsSystem(); // 构造函数
        void updateVelocity(PhysicsData& data, float dt);
        void updatePosition(PhysicsData& data, float dt); //更新坐标
        bool isCollide(sf::FloatRect boxA, sf::FloatRect boxB); //判断碰撞箱是否接触
};