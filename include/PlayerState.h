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
    bool isWall;        // 正在墙边
    bool isRising;      // 正在上升
    bool isCrouch;      // 正在蹲下
    bool isRolling;     // 正在翻滚
    bool isStartJump;   // 开始跳跃
    bool isAttack;      // 正在攻击
    bool isCounter;     // 正在盾反
    bool isFacingRight; // 面向右边
    bool isStartRoll;   // 开始翻滚
    PlayerState();
};

enum class PlayerAnimAction
{
    // 仅仅是一些瞬时动作
    Counter = 5,
    Attack = 4,
    Roll = 3,
    None = 0
};

enum class PlayerLoopAnimAction
{

};

int toInt(PlayerAnimAction); // 辅助函数