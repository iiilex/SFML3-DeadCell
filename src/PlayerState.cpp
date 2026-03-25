#include <PlayerState.h>

PlayerState::PlayerState()
{
    isMoving = false;
    isGrounded = false;
    isWall = false;
    isRising = false;
    isCrouch = false;
    isAttack = false;
    isCounter = false;
    isRolling = false;
    isFacingRight = true;
    isStartJump = false;
    isStartRoll = false;
}

int toInt(PlayerAnimAction thisAction)
{
    return static_cast<int>(thisAction);
}