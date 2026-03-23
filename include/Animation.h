#pragma once

#include<SFML/Graphics.hpp>
#include<string>
#include<vector>
#include<unordered_map>
#include<optional>

#include<PlayerState.h>

struct AnimAction
{
    int frameCnt; //当前动作的帧数量
    std::vector<sf::Texture> actionSequence; //动作序列
    bool isLoop; // 是否为循环动作
    bool isReversed; // 是否为某一素材库的反转
    bool isTransition; // 是否为过渡动作
    std::string nextAction; // 假如是过渡动作那么下一个动作是啥
};

class AnimPlayer
{
    protected:
        float timeAccumulator; // 时间累计器
        int frameId; // 当前播放到的帧序号
        bool isNewCalling; // 当前动作是否刚刚被唤醒
    public:
        bool isFinished; // 当前动作是否播放完毕
        AnimAction myAction; //这个播放器绑定的动作属性
        std::optional<sf::Sprite> nowSprite; // 目前的精灵图
        AnimPlayer();
        void reset();
        sf::Sprite& update(float dt);
        void registerAction(std::string& path);
}; // 作为资源

class AnimSystem
{
    protected:
        int animPlayerCnt;
        std::unordered_map<std::string, AnimPlayer> animPlayers;
        std::string nowAnimState;
    public:
        AnimSystem();
        void registerPlayer(std::string& path, std::string& name,std::string& sourceName, int isLoop, int isReversed,  int isTransition, std::string& nextAnim);
        void changePlayer(std::string newPlayerName);
        sf::Sprite& updatePlayer(float dt);
        sf::Sprite getCurrentSprite();
        bool isFinished();
        void turnToNextAction();
};
