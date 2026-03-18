#include <Player.h>

const float basic_velocity_x = 400;
const float basic_velocity_y = -1000;

Player::Player() : myShape({50, 50})
{
    myShape.setFillColor(sf::Color::Yellow);
    myShape.setPosition({0, window_size_y - 50});
    myData.box = myShape.getGlobalBounds();
    myData.position = myShape.getPosition();
    myData.velocity = {0, 0};
    myData.isLanded = true;
    myData.isWalled = false;
    direction = 1;
    jumpCnt = 0;
}

void Player::update(sf::RenderWindow &window, PhysicsSystem &mySystem, float dt)
{
    if (!stateQuery.empty())
    {
        auto nowState = stateQuery.front();
        stateQuery.pop();

        switch (nowState)
        {
        case PlayerState::Run:
            myData.velocity.x = basic_velocity_x * direction;
            break;
        case PlayerState::EndRun:
            myData.velocity.x = 0;
            break;
        case PlayerState::Jump:
            if(jumpCnt<2)
            {
                myData.velocity.y = basic_velocity_y;
                jumpCnt ++;
            }
            break;
        }
    }
    

    mySystem.updatePosition(myData, dt);
    if(myData.isLanded)  jumpCnt = 0;
    mySystem.updateVelocity(myData, dt);
    myShape.setPosition(myData.position);
}

void Player::jump()
{
    stateQuery.push(PlayerState::Jump);
}

void Player::runLeft()
{
    direction = -1;
    if(!stateQuery.empty() && stateQuery.front() == PlayerState::Run)  return ;
    stateQuery.push(PlayerState::Run);
}

void Player::runRight()
{
    direction = 1;
    if(!stateQuery.empty() && stateQuery.front() == PlayerState::Run)  return ;
    stateQuery.push(PlayerState::Run);
}

void Player::endRun()
{
    stateQuery.push(PlayerState::EndRun);
}

void Player::draw(sf::RenderWindow &window)
{
    window.clear();
    window.draw(myShape);
    window.display();
}