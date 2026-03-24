#include <iostream>

#include <player.h>

InputSection::InputSection()
{
    isLeftPressed = false;
    isRightPressed = false;
}

Player::Player() : section(),
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
    if (flag)
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
    if (flag)
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

void Player::updateInputSection(float dt)
{
    while (!section.inputBuffer.empty())
    {
        auto &item = section.inputBuffer.front();
        if (item.second <= dt)
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
    if (thisAction == InputAction::Counter)
        return 5;
    if (thisAction == InputAction::Roll)
        return 4;
    if (thisAction == InputAction::Attack)
        return 3;
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

void Player::reupdatePlayerState(const PhysicsData &data)
{
    if (state.isMoving && data.velocity.x == 0)
    {
        state.isMoving = false;
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

PlayerAnimAction Player::getNextAnimAction()
{
    if (state.isCounter)
        return PlayerAnimAction::Counter;
    if (state.isAttack)
        return PlayerAnimAction::Attack;
    if (state.isRolling)
        return PlayerAnimAction::Roll;
    if (state.isRising)
        return PlayerAnimAction::Jump;
    if (!state.isRising && !state.isGrounded)
        return PlayerAnimAction::Fall;
    if (state.isCrouch)
        return PlayerAnimAction::Crouch;
    if (state.isMoving)
        return PlayerAnimAction::Run;
    return PlayerAnimAction::Idle;
}

void Player::updatePlayer(float dt, PhysicsSystem &system, sf::RenderWindow &window)
{
    updateInputSection(dt); // 更新输入序列
    updatePlayerState();    // 依照输入序列更新角色状态和物理状态

    playerData.box = playerAnimSystem.getCurrentSprite().getLocalBounds();

    system.updateVelocity(playerData, dt); // 更新速度
    system.updatePosition(playerData, dt); // 更新位置

    reupdatePlayerState(playerData); // 由当前速度和位置再次更新角色状态

    // --- 修改一下动画更新 ---
    auto nextAction = getNextAnimAction();

    // 获取优先级
    int nextPrio = getPriority(nextAction);
    int currPrio = getPriority(currentAction);

    bool shouldSwitch = false;

    // 1. 如果新动作优先级更高，必须切换 (如攻击、受伤)
    if (nextPrio > currPrio)
    {
        shouldSwitch = true;
    }
    // 2. 如果动作改变了，且当前动作是“可中断”的 (如 Run, Jump)，也应该允许切换到低优先级的 Idle
    else if (nextAction != currentAction)
    {
        // 定义哪些动作是可以被随时打断的 (通常是移动和下落)
        bool isCurrentInterruptible = (currentAction == PlayerAnimAction::Run ||
                                       currentAction == PlayerAnimAction::Fall ||
                                       currentAction == PlayerAnimAction::Crouch); // 如果有 Walk

        // 如果当前是可中断的，或者新动作优先级不低于当前，就切换
        if (isCurrentInterruptible || nextPrio >= currPrio)
        {
            shouldSwitch = true;
        }
    }

    if (shouldSwitch)
    {
        std::string transitionName = getTransition(currentAction, nextAction);
        if (transitionName == "none")
            transitionName = getPlayerAnimActionName(nextAction);

        // 切换动画
        playerAnimSystem.changePlayer(transitionName);

        // 【关键】：切换后，确保动画系统知道这是一个新开始，可能需要重置 isFinished 标志
        // 如果你的 changePlayer 内部没有重置，可能需要手动调一下，或者确保 changePlayer 实现正确
        currentAction = nextAction;
    }

    // 原来的播放完毕逻辑保留，用于处理非循环动画的自然过渡
    if (playerAnimSystem.isFinished())
    {
        playerAnimSystem.turnToNextAction();
    }

    // 简陋测试作画部分
    playerAnimSystem.updatePlayer(dt);
    sf::Sprite nowPlayerSprite = playerAnimSystem.getCurrentSprite();
    nowPlayerSprite.setPosition(playerData.position);
    if (!state.isFacingRight)
        nowPlayerSprite.setScale({-1.f, 1.f});
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
    while (inFile >> animName)
    {
        std::string sourceName = "none";
        std::string nextAnim = "none";
        inFile >> isLoop >> isReversed;
        if (isReversed)
            inFile >> sourceName;
        inFile >> isTransition;
        if (isTransition)
            inFile >> nextAnim;
        playerAnimSystem.registerPlayer(rootPath, animName, sourceName, isLoop, isReversed, isTransition, nextAnim);
    }
}
