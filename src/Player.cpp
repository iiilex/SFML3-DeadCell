#include<iostream>

#include<player.h>

InputSection::InputSection()
{
    isLeftPressed = false;
    isRightPressed = false;
}

Player::Player():
section(),
state(),
playerData(),
playerAnimSystem(),
currentAction(PlayerAnimAction::Idle)
{
    initAnimSystem();
}

void Player::leftPressed(bool flag)
{
    section.isLeftPressed = flag;
    if(flag)
    {
        state.isMoving = true;
        playerData.velocity.x = -default_velocity_x;
        state.isFacingRight = false;
    }
    else
    {
        state.isMoving = false;
        playerData.velocity.x = 0;
    }
}

void Player::rightPressed(bool flag)
{
    section.isRightPressed = flag;
    if(flag)
    {
        state.isMoving = true;
        playerData.velocity.x = default_velocity_x;
        state.isFacingRight = true;
    }
    else
    {
        state.isMoving = false;
        playerData.velocity.x = 0;
    }
}

void Player::crouchPressed(bool flag)
{
    section.isCrouchPressed = flag;
    if(flag)
    {
        state.isMoving = false;
        state.isCrouch = true;
        playerData.velocity.x = 0;
    }
    else
    {
        state.isCrouch = false;
    }
}

void Player::jump()
{
    insertAction(InputAction::Jump);
}

void Player::roll()
{
    insertAction(InputAction::Roll);
}

void Player::attack()
{
    insertAction(InputAction::Attack);
}

void Player::counter()
{
    insertAction(InputAction::Counter);
}

void Player::insertAction(const InputAction newAction,const float duration)
{
    section.inputBuffer.push_back({newAction, duration});
}

void Player::updateInputSection(float dt)
{
    while(!section.inputBuffer.empty())
    {
        auto& item = section.inputBuffer.front();
        if(item.second <= dt)
        {
            section.inputBuffer.pop_front();
        }
        else
        {
            item.second -= dt;
        }
    }
}

// 辅助函数：得到动作的优先级
int getPriority(InputAction thisAction)
{
    if(thisAction == InputAction::Counter)  return 5;
    if(thisAction == InputAction::Roll)  return 4;
    if(thisAction == InputAction::Attack)  return 3;
}

InputAction Player::getCurrentInputAction()
{
    InputAction res = InputAction::None;
    int maxLevel = -1;
    for(const auto &item : section.inputBuffer)
    {
        if(item.second >= maxLevel)
        {
            maxLevel = item.second;
            res = item.first;
        }
    }
    return res;
}

void Player::updatePlayerState()
{
    auto nowAction = getCurrentInputAction();
    if(nowAction == InputAction::None)
    {
        return ;
    }
    switch(nowAction)
    {
        case InputAction::Counter:
            state.isMoving = false;
            state.isRolling = false;
            state.isCrouch = false;
            state.isCounter = true;
            playerData.velocity.x = 0;
            break;
        case InputAction::Attack:
            state.isMoving = false;
            state.isRolling = false;
            state.isCrouch = false;
            state.isAttack = true;
            playerData.velocity.x = 0;
            break;
        case InputAction::Roll:
            state.isCrouch = false;
            state.isRolling = true;
            state.isMoving = true;
            playerData.velocity.x = default_rolling_velocity_x;
            break;
        case InputAction::Jump:
            state.isRising = true;
            state.isGrounded = false;
            playerData.velocity.y = default_velocity_y;
            break;
    }
}

void Player::reupdatePlayerState(const PhysicsData& data)
{
    if(state.isMoving && data.velocity.x == 0)
    {
        state.isMoving = false;
    }
    if(data.velocity.y == 0)
    {
        if(state.isRising)  
        {
            state.isRising = false;
        }
        else if(!state.isGrounded)
        {
            state.isGrounded = true;
        }
    }
    else if(data.velocity.y < 0)
    {
        state.isRising = false;
    }
}

PlayerAnimAction Player::getNextAnimAction()
{
    if(state.isCounter)  return PlayerAnimAction::Counter;
    if(state.isAttack)  return PlayerAnimAction::Attack;
    if(state.isRolling)  return PlayerAnimAction::Roll;
    if(state.isRising)  return PlayerAnimAction::Jump;
    if(!state.isRising && !state.isGrounded)  return PlayerAnimAction::Fall;
    if(state.isCrouch)  return PlayerAnimAction::Crouch;
    if(state.isMoving)  return PlayerAnimAction::Run;
    return PlayerAnimAction::Idle;
}

void Player::updatePlayer(float dt, PhysicsSystem& system, sf::RenderWindow& window)
{
    updateInputSection(dt); // 更新输入序列
    updatePlayerState(); // 依照输入序列更新角色状态和物理状态

    playerData.box = playerAnimSystem.getCurrentSprite().getLocalBounds();

    system.updateVelocity(playerData, dt); // 更新速度
    system.updatePosition(playerData, dt); // 更新位置

    reupdatePlayerState(playerData); // 由当前速度和位置再次更新角色状态

    auto nextAction = getNextAnimAction();

    if(nextAction != currentAction && getPriority(nextAction) >= getPriority(currentAction))
    {
        std::string transitionName = getTransition(currentAction, nextAction);
        if(transitionName == "none")
            transitionName = getPlayerAnimActionName(nextAction);
        playerAnimSystem.changePlayer(transitionName);
    }

    if(playerAnimSystem.isFinished())
    {
        playerAnimSystem.turnToNextAction();
    }

    // 简陋测试作画部分
    sf::Sprite nowPlayerSprite = playerAnimSystem.updatePlayer(dt);
    nowPlayerSprite.setPosition(playerData.position);
    if(!state.isFacingRight)  nowPlayerSprite.setScale({-1.f,1.f});
    window.clear();
    window.draw(nowPlayerSprite);
    window.display();

}

// --- 动画资源相关 ---

void Player::initAnimSystem()
{
    std::ifstream inFile("./data/PlayerAnim.txt");
    std::string animName;
    std::string rootPath = "./atlas/player";
    int isLoop, isReversed, isTransition;
    while(inFile >> animName)
    {
        std::string sourceName = "none";
        std::string nextAnim = "none";
        inFile >> isLoop >> isReversed;
        if(isReversed)  inFile >> sourceName;
        inFile >> isTransition;
        if(isTransition)  inFile >> nextAnim;
        playerAnimSystem.registerPlayer(rootPath, animName, sourceName, isLoop, isReversed, isTransition, nextAnim);
    }
}

