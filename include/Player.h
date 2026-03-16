#include<SFML/Graphics.hpp>
#include<queue>

#include<Physics.h>
#include<Global.h>

using std::queue;

extern enum class PlayerState
{
    Run, //跑步
    EndRun, //结束跑步
    Jump,  //跳跃
    Roll, // 移动
    Crouch // 蹲下
};

class Player
{
    protected:
        PhysicsData myData;
        queue<PlayerState> stateQuery;
        int direction; //1表示右，-1表示左
    
        sf::RectangleShape myShape;
    public:
        Player();
        void update(sf::RenderWindow &window, PhysicsSystem& mySystem, float dt);

        void jump();
        void runLeft();
        void runRight();
        void endRun();
};

