#include <PlayerState.h>

PlayerState::PlayerState()
{
    isMoving = false;
    isGrounded = false;
    isRising = false;
    isCrouch = false;
    isAttack = false;
    isCounter = false;
    isRolling = false;
    isFacingRight = true;
}

int getPriority(const PlayerAnimAction a)
{
    if(a == PlayerAnimAction::Counter)  return 5;
    if(a == PlayerAnimAction::Roll)  return 4;
    if(a == PlayerAnimAction::Attack)  return 4;
    if(a == PlayerAnimAction::Jump)  return 3;
    if(a == PlayerAnimAction::Fall)  return 3;
    if(a == PlayerAnimAction::Crouch)  return 2;
    if(a == PlayerAnimAction::Run)  return 1;
    if(a == PlayerAnimAction::Idle)  return 0;
}

// 这是通过动作类型得到动作开始的资源名称,用于找不到衔接点的更新
std::string getPlayerAnimActionName(PlayerAnimAction thisAction)
{
    if(thisAction == PlayerAnimAction::Counter)  return "blockEnd";
    if(thisAction == PlayerAnimAction::Roll)  return "rollStart";
    if(thisAction == PlayerAnimAction::Attack)  return "atkCodgA";
    if(thisAction == PlayerAnimAction::Jump)  return "jumpThrough";
    if(thisAction == PlayerAnimAction::Fall)  return "jumpFalling";
    if(thisAction == PlayerAnimAction::Crouch)  return "crouch";
    if(thisAction == PlayerAnimAction::Run)  return "run";
    if(thisAction == PlayerAnimAction::Idle)  return "idle";
}

// 这是通过两种类型得到中间的衔接点，如果没有衔接就是 none
std::string getTransition(const PlayerAnimAction currentAction, const PlayerAnimAction nextAction)
{
    using PlayerAnimAction::Counter;
    using PlayerAnimAction::Roll;
    using PlayerAnimAction::Attack;
    using PlayerAnimAction::Jump;
    using PlayerAnimAction::Fall;
    using PlayerAnimAction::Crouch;
    using PlayerAnimAction::Run;
    using PlayerAnimAction::Idle;

    if(currentAction == Idle && nextAction == Run)  return "idleRun";
    if(currentAction == Idle && nextAction == Crouch)  return "idleCrouch";
    if(currentAction == Run && nextAction == Idle)  return "runIdle";
    if(currentAction == Crouch && nextAction == Idle)  return "crouchIdle";
    if(currentAction == Roll && nextAction == Idle)  return "rollIdle";
    return "none";
}