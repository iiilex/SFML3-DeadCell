#include<SFML/Graphics.hpp>
#include<queue>
#include<string>
#include<unordered_map>

#include<Physics.h>
#include<Animation.h>
#include<Global.h>

using std::queue;
using std::string;
using std::unordered_map;

enum class PlayerState
{
    None = 0, // 占位符号
    Idle = 1, // 静止
    Run = 2, //跑步
    EndRun = 3, //结束跑步
    Jump = 4,  //跳跃
    Roll = 5, // 翻滚
    Crouch = 6, // 蹲下
    EndCrouch = 7
};

// not_interupted[i][j] 从状态i不能变到状态j

enum class PlayerAnimState
{
    none = 0, // 占位的
    idle = 1,
    idleRun = 2,
    runIdle = 3,
    run = 4,
    dash = 5,
    endDashRun = 6,
    jumpThrough = 7,
    jumpTransition = 8,
    crouch = 9,
    crouchIdle = 10,
    idleCrouch = 11,
    rollStart = 12,
    rolling = 13,
    rollEnd = 14,
    rollIdle = 15,
    rollRun = 16,
    atkCodgA = 17,
    atkCodgB = 18,
    atkCodgC = 19,
    jumpFalling = 20,
    _count // 计数用的
}; // 1~19

class Player
{
    protected:
        PhysicsData myData;

        queue<PlayerState> stateQuery;
        PlayerState lastState;
        PlayerState nowState;

        int direction; //1表示右，-1表示左
        int jumpCnt; //连跳计数
        int hitCnt; //连击计数
        bool notAllowed[25][25]; // notAllowed[i][j] 表示动作i不能流向j（除非动作结束）

        AnimData<PlayerAnimState> myAnimData;
        AnimSystem<PlayerAnimState> myAnimSystem;
    public:
        Player();
        void animInit();

        void update(sf::RenderWindow &window, PhysicsSystem& mySystem, float dt);
        void draw(sf::RenderWindow &window, sf::Sprite &sprite);

        void jump();
        void runLeft();
        void runRight();
        void endRun();
        void roll();
        void crouch();
        void endCrouch();
};

