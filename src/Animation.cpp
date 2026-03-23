#include<Animation.h>
#include<filesystem>

const float frame_duration = 0.028;

AnimPlayer::AnimPlayer(): 
isFinished(false),
isNewCalling(true),
frameId(0),
timeAccumulator(0.f)
{}

sf::Sprite& AnimPlayer::update(float dt)
{
    if(isFinished && !myAction.isLoop || isNewCalling)
    {
        isNewCalling = false;
        return nowSprite.value();
    }
    timeAccumulator += dt;
    while(timeAccumulator >= frame_duration)
    {
        timeAccumulator -= frame_duration;
        frameId ++;
        if(frameId >= myAction.frameCnt)
        {
            if(myAction.isLoop)
            {
                frameId = 0;
            }
            else
            {
                isFinished = 1;
                frameId = myAction.frameCnt-1;
                break;
            }
        }
    }
    nowSprite.value().setTexture(myAction.actionSequence[frameId], true);
    return nowSprite.value();
}

void AnimPlayer::reset()
{
    frameId = 0;
    isFinished = false;
    isNewCalling = true;
    timeAccumulator = 0;
}

AnimSystem::AnimSystem() :
animPlayerCnt(0),
nowAnimState("idle")
{}

// 辅助函数：统计地址下图片的数量
int picCount(std::string& path)
{
    int res = 0;
    for (const auto& entry : std::filesystem::directory_iterator(path)) 
        res ++;
    return res;
}
// 辅助函数：int 1 转换为 string 001
std::string toString(int x)
{
    std::string res = "000";
    res[0] = x/100 + '0';
    res[1] = x/10%10 + '0';
    res[2] = x%10 + '0';
    return res;
}
// 辅助函数结束

void AnimPlayer::registerAction(std::string& path)
{
    myAction.frameCnt = picCount(path);
    for(int i=0; i<myAction.frameCnt; i++)
    {
        std::string fullPath = path + '/' + toString(i) + ".png";
        myAction.actionSequence.push_back(sf::Texture(fullPath));
    }
    if(myAction.isReversed)
    {
        for(int i=0, j =myAction.frameCnt-1; i<j; i++, j--)
        {
            auto temp = myAction.actionSequence[i];
            myAction.actionSequence[i] = myAction.actionSequence[j];
            myAction.actionSequence[j] = temp;
        }
    }
    nowSprite.emplace(myAction.actionSequence[0]);
}

void AnimSystem::registerPlayer(std::string& path, std::string& name, std::string& sourceName, int isLoop, int isReversed, int isTransition, std::string& nextAnim)
{
    if(sourceName == "none")  sourceName = name;
    std::string realPath = path + '/' + sourceName;
    animPlayers[name].myAction.isLoop = isLoop;
    animPlayers[name].myAction.isReversed = isReversed;
    animPlayers[name].myAction.isTransition = isTransition;
    animPlayers[name].myAction.nextAction = nextAnim;
    animPlayers[name].registerAction(realPath);
    animPlayers[name].nowSprite.emplace(animPlayers[name].myAction.actionSequence[0]);
}

void AnimSystem::changePlayer(std::string newPlayerName)
{
    nowAnimState = newPlayerName;
    animPlayers[nowAnimState].reset();
}

sf::Sprite& AnimSystem::updatePlayer(float dt)
{
    return animPlayers[nowAnimState].update(dt);
}

sf::Sprite AnimSystem::getCurrentSprite()
{
    return animPlayers[nowAnimState].nowSprite.value();
}

bool AnimSystem::isFinished()
{
    return animPlayers[nowAnimState].isFinished;
}

void AnimSystem::turnToNextAction()
{
    nowAnimState = animPlayers[nowAnimState].myAction.nextAction;
    if(nowAnimState == "none")  nowAnimState = "idle";
}