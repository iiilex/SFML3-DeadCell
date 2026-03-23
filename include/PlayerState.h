#pragma once

#include<unordered_map>
#include<string>

enum class InputAction
{
    None,
    Jump,
    Roll,
    Crouch,
    Attack,
    Counter
};

class PlayerState
{
public:
    bool isMoving;      // 正在移动
    bool isGrounded;    // 正在地面
    bool isRising;      // 正在上升
    bool isCrouch;      // 正在蹲下
    bool isRolling;     // 正在翻滚
    bool isAttack;      // 正在攻击
    bool isCounter;     // 正在盾反
    bool isFacingRight; // 面向右边
    PlayerState();
};

enum class PlayerAnimAction
{
    Counter,
    Roll, Attack,
    Jump, Fall,
    Crouch,
    Run,
    Idle
};

int getPriority(const PlayerAnimAction a);

std::string getPlayerAnimActionName(PlayerAnimAction thisAction);
std::string getTransition(const PlayerAnimAction currentAction, const PlayerAnimAction nextAction);