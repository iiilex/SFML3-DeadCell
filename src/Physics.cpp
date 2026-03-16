#include<Physics.h>

const float max_velocity_y = 1000;

PhysicsSystem::PhysicsSystem()
{

}

void PhysicsSystem::updateVelocity(PhysicsData& data, float dt)
{
    if(data.isLanded)
    {
        data.velocity.y = 0;
        return ;
    }
    data.velocity.y += gravity * dt;
    if(data.velocity.y > max_velocity_y) 
        data.velocity.y = max_velocity_y;
}

void PhysicsSystem::updatePosition(PhysicsData& data , float dt)
{
    data.position.x += data.velocity.x * dt;
    data.position.y += data.velocity.y * dt;

    //在这里写入坐标修正的方法，只是为了测试
    //后面肯定会改的

    data.isLanded = false;
    if(data.position.y > window_size_y-50)
    {
        data.position.y = window_size_y-50;
        data.isLanded = true;
    }
    if(data.position.x < 0 )
    {
        data.position.x = 0;
        data.isWalled = true;
    }
    if(data.position.x > window_size_x - 50)
    {
        data.position.x = window_size_x - 50;
        data.isWalled = true;
    }

}

bool PhysicsSystem::isCollide(sf::FloatRect boxA, sf::FloatRect boxB)
{
    return (boxA.position.x < boxB.position.x + boxB.size.x && boxB.position.x < boxA.position.x + boxA.size.x) &&
           (boxA.position.y < boxB.position.y + boxB.size.y && boxB.position.y < boxA.position.y + boxA.size.y);
} //AABB简单检测