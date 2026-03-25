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
const float default_rolling_velocity_x = 1800;
const float default_velocity_y = -1000;

class InputSection
{
public:
    std::deque<std::pair<InputAction, float>> inputBuffer;
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

    AnimSystem playerAnimSystem; //非循环动画管理
    AnimSystem playerLoopAnimSystem; // 循环动画管理

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
    void insertAction(const InputAction newAction, const float duration = default_duration); // 插入动作
    void updateInputSection(float dt); // 更新当前输入缓冲序列
    InputAction getCurrentInputAction(); // 得到输入序列中应该处理的动作
    void updatePlayerState(); // 更新角色当前的一系列状态
    void reupdatePlayerState(const PhysicsData& data); // 通过物理规则再校正
    // 动画相关
    void initAnimSystem(); // 初始化动画系统
    void draw(sf::RenderWindow& window); // 绘制角色
    void updatePlayerAnimSystem(float dt); // 更新非循环动画机
    void updatePlayerLoopAnimSystem(float dt); // 更新循环动画机
    sf::Sprite getCurrentPlayerSprite(); // 得到当前角色的精灵图

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