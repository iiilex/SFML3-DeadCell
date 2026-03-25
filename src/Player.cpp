#include <iostream>
#include <thread>
#include <chrono>

#include <player.h>

// ---------------- 按键检测相关 ----------------

InputSection::InputSection()
{
    isLeftPressed = false;
    isRightPressed = false;
}

Player::Player() : section(),
                   state(),
                   playerData(),
                   playerAnimSystem(),
                   currentAction(PlayerAnimAction::None)
{
    initAnimSystem();
}

void Player::leftPressed(bool flag)
{
    section.isLeftPressed = flag;
    if (flag && (!state.isWall || state.isWall && state.isFacingRight))
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
    if (flag && (!state.isWall || state.isWall && !state.isFacingRight))
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
    if (flag)
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

void Player::insertAction(const InputAction newAction, const float duration)
{
    section.inputBuffer.push_back({newAction, duration});
}

// ---------------- update相关 ----------------

void Player::updateInputSection(float dt)
{
    if (section.inputBuffer.empty())
        return;
    for (auto &item : section.inputBuffer)
    {
        item.second -= dt;
    }
    auto it = section.inputBuffer.begin();
    while (it->second <= 0)
    {
        section.inputBuffer.pop_front();
        if (section.inputBuffer.empty())
            break;
        it = section.inputBuffer.begin();
    }
}

InputAction Player::getCurrentInputAction()
{
    InputAction res = InputAction::None;
    int maxLevel = -1;
    for (const auto &item : section.inputBuffer)
    {
        if (item.second >= maxLevel)
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
    if (nowAction == InputAction::None)
    {
        return;
    }

    switch (nowAction)
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
        state.isStartRoll = true;
        state.isRolling = true;
        state.isMoving = true;
        if (state.isFacingRight)
            playerData.velocity.x = default_rolling_velocity_x;
        else
            playerData.velocity.x = -default_rolling_velocity_x;
        break;
    case InputAction::Jump:
        state.isStartJump = true;
        state.isRising = true;
        state.isGrounded = false;
        playerData.velocity.y = default_velocity_y;
        break;
    }
}

void Player::reupdatePlayerState(const PhysicsData &data)
{
    if (state.isMoving)
    {
        if (data.velocity.x == 0)
        {
            state.isMoving = false;
            state.isWall = true;
        }
        else
        {
            state.isMoving = true;
            state.isWall = false;
        }
    }

    if (data.velocity.y == 0)
    {
        if (state.isRising)
        {
            state.isRising = false;
        }
        else if (!state.isGrounded)
        {
            state.isGrounded = true;
        }
    }
    else if (data.velocity.y < 0)
    {
        state.isRising = false;
    }
}

void Player::updatePlayer(float dt, PhysicsSystem &system, sf::RenderWindow &window)
{

    updateInputSection(dt); // 更新输入序列

    updatePlayerState(); // 依照输入序列更新角色状态和物理状态

    auto nowSprite = getCurrentPlayerSprite();
    playerData.box = nowSprite.getGlobalBounds();

    system.updateVelocity(playerData, dt); // 更新速度
    system.updatePosition(playerData, dt); // 更新位置

    playerAnimSystem.setPos(playerData.position);
    playerLoopAnimSystem.setPos(playerData.position);

    reupdatePlayerState(playerData); // 由当前速度和位置再次更新角色状态

    // --- 修改一下动画更新 ---

    updatePlayerAnimSystem(dt);

    updatePlayerLoopAnimSystem(dt);

    draw(window);
}

// ---------------- 动画资源相关 ----------------

void Player::initAnimSystem()
{
    std::ifstream inFile("./data/PlayerAnim.txt");
    std::string animName;
    std::string rootPath = "./atlas/player";
    int isLoop, isReversed, isTransition;
    int loopAnimCnt, unloopAnimCnt;
    inFile >> loopAnimCnt >> unloopAnimCnt;

    while (loopAnimCnt--)
    {
        std::string sourceName = "none";
        std::string nextAnim = "none";
        inFile >> animName >> isLoop >> isReversed;
        if (isReversed)
            inFile >> sourceName;
        inFile >> isTransition;
        if (isTransition)
            inFile >> nextAnim;
        playerLoopAnimSystem.registerPlayer(rootPath, animName, sourceName, isLoop, isReversed, isTransition, nextAnim);
    }

    while (unloopAnimCnt--)
    {
        std::string sourceName = "none";
        std::string nextAnim = "none";
        inFile >> animName >> isLoop >> isReversed;
        if (isReversed)
            inFile >> sourceName;
        inFile >> isTransition;
        if (isTransition)
            inFile >> nextAnim;
        playerAnimSystem.registerPlayer(rootPath, animName, sourceName, isLoop, isReversed, isTransition, nextAnim);
    }

    playerAnimSystem.changePlayer("none");
    playerLoopAnimSystem.changePlayer("idle");
}

sf::Sprite Player::getCurrentPlayerSprite()
{
    if (!playerAnimSystem.isEmpty())
    {
        return playerAnimSystem.getCurrentSprite(state.isFacingRight);
    }
    else
    {
        return playerLoopAnimSystem.getCurrentSprite(state.isFacingRight);
    }
}

void Player::draw(sf::RenderWindow &window) // 注意这只是一个简单测试单元
{
    auto shape = getCurrentPlayerSprite();

    window.clear();
    window.draw(shape);
    window.display();
}

void Player::updatePlayerAnimSystem(float dt)
{
    if(state.isCounter)
    {
        state.isCounter = false;
        currentAction = PlayerAnimAction::Counter;
        playerAnimSystem.changePlayer("blockEnd");
    }
    else if(state.isAttack)
    {
        state.isAttack = false;
        if(toInt(currentAction) < toInt(PlayerAnimAction::Attack))
        {
            currentAction = PlayerAnimAction::Attack;
            playerAnimSystem.changePlayer("atkCodgA");
        }
    }
    else if(state.isStartJump)
    {
        state.isStartJump = false;
        if(playerAnimSystem.getName() == "roll")
            playerAnimSystem.changePlayer("idle");
    }
    else if(state.isRolling)
    {
        state.isRolling = false;
        if(toInt(currentAction) < toInt(PlayerAnimAction::Roll) && !state.isRising)
        {
            currentAction = PlayerAnimAction::Roll;
            playerAnimSystem.changePlayer("roll");
        }
    }
    
    playerAnimSystem.updatePlayer(dt);

    if(playerAnimSystem.isFinished())
    {
        currentAction = PlayerAnimAction::None;
        playerAnimSystem.changePlayer("none");
    }
}

void Player::updatePlayerLoopAnimSystem(float dt)
{
    if(state.isRising)
    {
        playerLoopAnimSystem.changePlayer("jumpRising");
    }
    else if(!state.isRising && !state.isGrounded)
    {
        playerLoopAnimSystem.changePlayer("jumpFalling");
    }
    else if(state.isMoving)
    {
        playerLoopAnimSystem.changePlayer("run");
    }
    else
    {
        playerLoopAnimSystem.changePlayer("idle");
    }

    playerLoopAnimSystem.updatePlayer(dt);
}