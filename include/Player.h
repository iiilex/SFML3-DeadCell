#pragma once

#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include <unordered_map>
#include <deque>

#include <Animation.h>
#include <Physics.h>
#include <PlayerState.h>

const float default_duration = 0.08;
const float default_velocity_x = 800;
const float default_rolling_velocity_x = 1400;
const float default_velocity_y = -1000;

class InputSection
{
public:
    std::deque<std::pair<InputAction, int>> inputBuffer;
    int bufferCnt;
    bool isLeftPressed;
    bool isRightPressed;
    bool isCrouchPressed;
    InputSection();
};

class Player
{
protected:
    InputSection section;
    PlayerState state;
    PhysicsData playerData;
    AnimSystem playerAnimSystem;

    PlayerAnimAction currentAction;

public:
    Player();

    // 接受键盘指令
    void leftPressed(bool flag);
    void rightPressed(bool flag);
    void crouchPressed(bool flag);
    void jump();
    void roll();
    void attack();
    void counter();

    // 当前动作相关
    void insertAction(const InputAction newAction, const float duration = default_duration);
    void updateInputSection(float dt);
    InputAction getCurrentInputAction();
    void updatePlayerState();
    void reupdatePlayerState(const PhysicsData& data);
    PlayerAnimAction getNextAnimAction();
    // 动画相关
    void initAnimSystem();
    void updatePlayer(float dt, PhysicsSystem& system, sf::RenderWindow& window);
};

/*
    键盘操作 -> 调用接口 -> 更新 InputSection
    player的update : 
    -> 通过 InputSection 更新一波角色动作状态 
    -> 根据 PhysicsSystem 更新物理状态
    -> 根据 物理状态 反更新角色状态
    -> 根据 角色状态 更新当前动作
*/