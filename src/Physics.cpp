#include<Physics.h>

const float max_velocity_y = 1000;

PhysicsData::PhysicsData()
{
    position = {0.f, 0.f};
    velocity = {0.f, 0.f};
    box = {{0.f, 0.f}, {0.f, 0.f}};
}

PhysicsSystem::PhysicsSystem()
{

}

void PhysicsSystem::updateVelocity(PhysicsData& data, float dt)
{
    data.velocity.y += gravity * dt;
    if(data.velocity.y > max_velocity_y) 
        data.velocity.y = max_velocity_y;
}

void PhysicsSystem::updatePosition(PhysicsData& data , float dt)
{
    data.position.x += data.velocity.x * dt;
    data.position.y += data.velocity.y * dt;

    if(data.position.y > window_size_y-data.box.size.y)
    {
        data.position.y = window_size_y - data.box.size.y;
        data.velocity.y = 0;
    }
    if(data.position.x < 0 )
    {
        data.position.x = 0;
        data.velocity.x = 0;
    }
    if(data.position.x > window_size_x - data.box.size.x)
    {
        data.position.x = window_size_x - data.box.size.x;
        data.velocity.x = 0;
    }

}

bool PhysicsSystem::isCollide(sf::FloatRect boxA, sf::FloatRect boxB)
{
    return (boxA.position.x < boxB.position.x + boxB.size.x && boxB.position.x < boxA.position.x + boxA.size.x) &&
           (boxA.position.y < boxB.position.y + boxB.size.y && boxB.position.y < boxA.position.y + boxA.size.y);
} //AABB简单检测