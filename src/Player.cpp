#include <Player.h>
#include <iostream>
#include <filesystem>
#include <string>

#include <chrono>
#include <thread>

using std::string;

const float basic_velocity_x = 800;
const float basic_velocity_y = -1000;
const float idle_time = 0.05;
const float player_scale = 3.5;

unordered_map<PlayerAnimState, string> animDir{
    {PlayerAnimState::idle, "idle"},
    {PlayerAnimState::idleRun, "idleRun"},
    {PlayerAnimState::runIdle, "idleRun"},
    {PlayerAnimState::run, "run"},
    {PlayerAnimState::dash, "dash"},
    {PlayerAnimState::endDashRun, "endDashRun"},
    {PlayerAnimState::jumpThrough, "jumpThrough"},
    {PlayerAnimState::jumpTransition, "jumpTransition"},
    {PlayerAnimState::jumpFalling, "jumpFalling"},
    {PlayerAnimState::crouch, "crouch"},
    {PlayerAnimState::crouchIdle, "crouchIdle"},
    {PlayerAnimState::idleCrouch, "crouchIdle"},
    {PlayerAnimState::atkCodgA, "atkCodgA"},
    {PlayerAnimState::atkCodgB, "atkCodgB"},
    {PlayerAnimState::atkCodgC, "atkCodgC"},
    {PlayerAnimState::rollStart, "rollStart"},
    {PlayerAnimState::rolling, "rolling"},
    {PlayerAnimState::rollEnd, "rollEnd"},
    {PlayerAnimState::rollIdle, "rollIdle"},
    {PlayerAnimState::rollRun, "rollRun"}};

unordered_map<PlayerAnimState, bool> animReverse{
    {PlayerAnimState::runIdle, true},
    {PlayerAnimState::idleCrouch, true}};

// 辅助函数：统计文件夹图片数量
int countFiles(const std::string &dir)
{
    try
    {
        int res = 0;
        // 如果这里抛出异常，说明 dir 路径有问题
        for (const auto &entry : std::filesystem::directory_iterator(dir))
        {
            res++;
        }
        return res;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        // === 关键调试信息 ===
        std::cerr << "【致命错误】文件系统操作失败!" << std::endl;
        std::cerr << "错误代码: " << e.code().message() << std::endl;
        std::cerr << "出错路径: " << e.path1().string() << std::endl;
        std::cerr << "完整信息: " << e.what() << std::endl;

        // 在这里打断点 (Debug -> Breakpoints -> Toggle Breakpoint)
        // 运行到这里时，你可以看到具体的路径是什么，为什么失败
        return 0; // 返回 0 防止程序崩溃，或者重新抛出
    }
}
// 辅助函数:把int 1转化为 string 001
string toString(int x)
{
    string res = "000";
    res[0] = x / 100 + '0';
    res[1] = x / 10 % 10 + '0';
    res[2] = x % 10 + '0';
    return res;
}
// 辅助函数：把 enum class 转化成 int
template <typename T>
int toInt(T x)
{
    return static_cast<int>(x);
}
// 辅助函数结束

Player::Player()
{
    myData.box = {{0,0},{80,80}};                 // 记得改
    myData.position = {0, 400}; // 记得改
    myData.velocity = {0, 0};
    myData.isLanded = false;
    myData.isWalled = false;
    myData.isFalling = true;

    lastState = nowState = PlayerState::Idle;

    direction = 1;
    jumpCnt = 0;

    animInit();

    int _Roll = toInt<PlayerState>(PlayerState::Roll);
    int _Run = toInt<PlayerState>(PlayerState::Run);
    int _EndRun = toInt<PlayerState>(PlayerState::EndRun);
    int _Jump = toInt<PlayerState>(PlayerState::Jump);
    int _Crouch = toInt<PlayerState>(PlayerState::Crouch);
    int _EndCrouch = toInt<PlayerState>(PlayerState::EndCrouch);
    int _Idle = toInt<PlayerState>(PlayerState::Idle);

    for (int i = 0; i <= 10; i++)
        for (int j = 0; j <= 10; j++)
            notAllowed[i][j] = 0;

    notAllowed[_Roll][_Run] = notAllowed[_Roll][_EndRun] = 1;
    notAllowed[_Roll][_Crouch] = notAllowed[_Roll][_EndCrouch] = 1;

    notAllowed[_Crouch][_Run] = notAllowed[_Crouch][_EndRun] = 1;
    notAllowed[_Jump][_Run] = notAllowed[_Jump][_EndRun] = 1;
    notAllowed[_Jump][_Crouch] = notAllowed[_Jump][_EndCrouch] = 1;

    notAllowed[_Run][_Run] = 1;
    notAllowed[_EndRun][_EndRun] = 1;
    notAllowed[_Crouch][_Crouch] = 1;
    notAllowed[_Idle][_Idle] = 1;
    notAllowed[_Idle][_EndRun] = notAllowed[_Idle][_EndCrouch] = 1;
    notAllowed[_EndRun][_EndCrouch] = notAllowed[_EndCrouch][_EndRun] = 1;
}

void Player::animInit()
{
    myAnimData.nowAnimState = PlayerAnimState::idle;

    myAnimData.isLoop[PlayerAnimState::idle] = 1;
    myAnimData.isLoop[PlayerAnimState::run] = 1;
    myAnimData.isLoop[PlayerAnimState::crouch] = 1;
    myAnimData.isLoop[PlayerAnimState::jumpFalling] = 1;

    myAnimData.id = 0;
    myAnimData.gotNew = 0;
    myAnimData.animQuery.push(PlayerAnimState::idle);
    string rootDir = "./atlas/player";
    for (int i = 1, len = static_cast<int>(PlayerAnimState::_count); i < len; i++)
    {
        PlayerAnimState t = static_cast<PlayerAnimState>(i);
        string Dir = rootDir + '/' + animDir[t];
        int picCnt = countFiles(Dir);
        myAnimData.animResourceCnt[t] = picCnt;
        myAnimData.animResource[t].resize(picCnt);
        for (int j = 0; j < picCnt; j++)
        {
            string fullDir = Dir + '/' + toString(j) + ".png";
            if (!animReverse[t])
                myAnimData.animResource[t][j].loadFromFile(fullDir);
            else
                myAnimData.animResource[t][picCnt - 1 - j].loadFromFile(fullDir);
        }
    }
}

void Player::update(sf::RenderWindow &window, PhysicsSystem &mySystem, float dt)
{
    auto tState = PlayerState::Idle;
    if (!stateQuery.empty())
    {
        tState = stateQuery.front();
        stateQuery.pop();
    }

    int _nowState = toInt<PlayerState>(nowState);
    int _tState = toInt<PlayerState>(tState);

    //std::this_thread::sleep_for(std::chrono::milliseconds(80));
    //std::cout<<_nowState<<" "<<_tState<<"\n";

    bool can_add = false;

    if (!notAllowed[_nowState][_tState])
    {
        std::queue<PlayerAnimState>().swap(myAnimData.animQuery);
        myAnimData.id = 0;
        myAnimData.gotNew = true;
        can_add = true;
        lastState = nowState;
        nowState = tState;
    }

    switch (nowState)
    {
    case PlayerState::Run:
        if (lastState == PlayerState::Crouch)
            break;

        myData.velocity.x = basic_velocity_x * direction;

        if (can_add)
        {
            if (lastState == PlayerState::Roll)
                myAnimData.animQuery.push(PlayerAnimState::rollRun);
            if (lastState == PlayerState::Idle)
                myAnimData.animQuery.push(PlayerAnimState::idleRun);
            myAnimData.animQuery.push(PlayerAnimState::run);
        }
        break;
    case PlayerState::EndRun:
        if (can_add)
        {
            myAnimData.animQuery.push(PlayerAnimState::runIdle);
            myAnimData.animQuery.push(PlayerAnimState::idle);
        }
        myData.velocity.x = 0;
        break;
    case PlayerState::Jump:
        if (can_add)
            myAnimData.animQuery.push(PlayerAnimState::jumpThrough);
        if (jumpCnt < 2)
        {
            myData.velocity.y = basic_velocity_y;
            jumpCnt++;
        }
        break;
    case PlayerState::Crouch:
        if (can_add)
        {
            myAnimData.animQuery.push(PlayerAnimState::idleCrouch);
            myAnimData.animQuery.push(PlayerAnimState::crouch);
        }
        break;
    case PlayerState::EndCrouch:
        if (can_add)
        {
            myAnimData.animQuery.push(PlayerAnimState::crouchIdle);
            myAnimData.animQuery.push(PlayerAnimState::idle);
        }
        break;
    case PlayerState::Roll:
        if (can_add)
        {
            myAnimData.animQuery.push(PlayerAnimState::rollStart);
            myAnimData.animQuery.push(PlayerAnimState::rolling);
            myAnimData.animQuery.push(PlayerAnimState::rollEnd);
        }
        break;
    case PlayerState::Idle:
        if (can_add)
        {
            myAnimData.animQuery.push(PlayerAnimState::idle);
        }
        break;
    }

    

    mySystem.updatePosition(myData, dt);
    if (myData.isLanded)
        jumpCnt = 0;
    mySystem.updateVelocity(myData, dt);

    if(myData.isFalling)  
    {
        myAnimData.animQuery.push(PlayerAnimState::jumpTransition);
        myAnimData.animQuery.push(PlayerAnimState::jumpFalling);
    }

    auto nowSprite = myAnimSystem.update(myAnimData, myData.position);

    if (direction == -1)
        nowSprite.setScale({-player_scale, player_scale});
    else
        nowSprite.setScale({player_scale, player_scale});
    nowSprite.setPosition(myData.position);
    myData.box = nowSprite.getGlobalBounds();

    draw(window, nowSprite);
}

void Player::jump()
{
    stateQuery.push(PlayerState::Jump);
}

void Player::runLeft()
{
    direction = -1;
    if (!stateQuery.empty() && nowState == PlayerState::Run)
        return;
    stateQuery.push(PlayerState::Run);
}

void Player::runRight()
{
    direction = 1;
    if (!stateQuery.empty() && nowState == PlayerState::Run)
        return;
    stateQuery.push(PlayerState::Run);
}

void Player::endRun()
{
    if (!stateQuery.empty() && (nowState == PlayerState::EndCrouch || nowState == PlayerState::EndRun || nowState == PlayerState::Idle))
        return;
    stateQuery.push(PlayerState::EndRun);
}

void Player::crouch()
{
    if (!stateQuery.empty() && nowState == PlayerState::Crouch)
        stateQuery.push(PlayerState::Crouch);
}

void Player::endCrouch()
{
    if (!stateQuery.empty() && (nowState == PlayerState::EndCrouch || nowState == PlayerState::EndRun || nowState == PlayerState::Idle))
        return;
    stateQuery.push(PlayerState::EndCrouch);
}

void Player::roll()
{
    stateQuery.push(PlayerState::Roll);
}

void Player::draw(sf::RenderWindow &window, sf::Sprite &sprite)
{
    window.clear();
    window.draw(sprite);
    window.display();
}
