#pragma once

#include <SFML/Graphics.hpp>
#include <queue>
#include <vector>
#include <unordered_map>

template <typename T>
struct AnimData
{
    std::queue<T> animQuery;
    std::unordered_map<T, std::vector<sf::Texture>> animResource;
    std::unordered_map<T, bool> isLoop;
    std::unordered_map<T, int> animResourceCnt;
    T nowAnimState;
    int id;
    bool gotNew;
};

const float clip_time = 0.028;

template <typename T>
class AnimSystem
{
protected:
    sf::Clock clock;
    float animTimer;

public:
    AnimSystem() : animTimer(-10000.f) {}
    sf::Sprite update(AnimData<T> &data, sf::Vector2f pos)
    {
        
        //std::cout << static_cast<int>(data.nowAnimState) << " " << data.id << " " << data.animResourceCnt[data.nowAnimState] << "\n";
        if(data.gotNew)
        {
            data.nowAnimState = data.animQuery.front();
            data.animQuery.pop();
            data.gotNew = 0;
        }
        float dt = clock.restart().asSeconds();
        animTimer += dt;
        if(animTimer<0)  animTimer = 0;
        while (animTimer >= clip_time)
        {
            animTimer -= clip_time;
            data.id++;
            if (data.id >= data.animResourceCnt[data.nowAnimState])
            {
                if (!data.isLoop[data.nowAnimState])
                {
                    if(!data.animQuery.empty())
                    {
                        data.nowAnimState = data.animQuery.front();
                        data.animQuery.pop();
                    }
                }
                data.id = 0;
            }
        }
        sf::Sprite res(data.animResource[data.nowAnimState][data.id]);
        
        res.setPosition(pos);
        return res;
    }
};
