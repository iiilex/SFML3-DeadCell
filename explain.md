# 我需要什么呢?

首先的首先，应当明确一个观念:

**系统是游戏整体的，资源是个体持有的**

## 一套物理系统

包括了在这个游戏世界中的各种物理模型

0. 世界中的物理参数
   1. 重力加速度
   2. 摩擦系数

1. 一套集成的物理参数 PhysicsData
   1. 世界中的坐标 (左上为(0,0), 向右为x,向下为y)
   2. 速度(x方向, y方向)
   3. 碰撞箱
   4. 是否撞墙
   5. 是否落地
   
2. 一套物理世界的判断更新规则 PhysicsSystem
   1. 根据重力加速度更新y方向速度
   2. 按照速度更新当前坐标
   3. 修正坐标(假如撞墙/撞到实体)
   4. 判断两个碰撞箱是否接触
   5. 判断当前物体是否着地(目前是简单版)
   6. 判断当前物体是否撞墙(目前是简单版)



## 一个角色 Player 类

0. 角色自己的状态包 struct playerState
   一些 bool 变量表示现在的状态(接受按键和物理规则更新后的结果，用于动画处理)
      isMoving 正在运动
      isGrounded 正在地上
      isRising 正在爬升(和下落互斥)
      isCrouch 正在下蹲
      isAttack 正在攻击
      isCounter 正在盾反

1. 输入序列 struct inputSection
   1. enum class inputAction 类的deque, 存储一小段时间内的输入，按照从早到晚的顺序更新状态，并且按照 当前更新的状态比之前所有更新的状态优先级不低 的 原则
   2. isLeftPressed/isRightPressed, 是否按下移动按键
   3. 

2. 初始化

3. 接受键盘的操作(压入动作缓冲序列)
   1. 跑步 Run
   2. 跳跃 Jump
   3. 翻滚 Roll
   4. 下蹲 Crouch
   5. 攻击 Attack
   6. 举盾 Counter

4. 角色自己的动作动画(未完待续，先做按键的)
   1. idle 静止
   2. idleRun 从静止到跑步
   3. runIdle (idleRun反向)
   4. run 普通跑步
   5. dash 冲刺
   6. endDashRun 从冲刺到奔跑
   7. jumpThrough 正常跳跃
   8. jumpTransition 正常跳跃跳跃结束后下落过程
   9. crouch 下蹲状态
   10. crouchIdle 蹲下到起立
   11. idleCrouch (crouchIdle反向)
   12. atkCodgA~C：砍刀与手枪攻击连招

## 一套动画系统

1. 单个动画动作的定义: AnimAction 类
   1. 当前动画包含帧数
   2. 类型为 sf::Texture 的完整动画素材序列
   3. 是否循环
   4. 是否倒序(一些动作素材是双向使用的,只在初始化用)

2. 用于播放动作的类型: AnimPlayer 类
   1. 一个 AnimAction 类 表示当前播放的动作
   2. 当前播放到第几帧
   3. 传出一个 sf::Sprite 给 AnimMachine 绘制当前动作的update函数
   4. 当前动作是否播放完了

3. 用于管理播放动作的类型: AnimMachine 类
   1. 状态之间的转移关系(从一个文件中读取 AnimState.txt, 播放完了就自动转到下个状态)
      (形似 myState["Alice"]["Bob"] = true)
   2. 类型为 哈希表 string-AnimAction 的映射，代表动作名称-动画动作的映射
   3. update
   
## 关于数据管理
1. PlayerAnim.txt : 定义角色的动画状态
2. 
   
明日计划:  
好好思考一下如何切换状态，尤其是过渡状态

给每个 AnimAction 加一个 bool 类型变量:是否为过渡状态
假如是过渡状态，那么就加一个 string 类型变量，表示这个状态应该过渡到什么状态
过渡状态的优先级跟过渡后达到的状态相同

所以，不止有角色的输入有优先级，动画之间也有优先级的，
这样可以规定某些动画应当插入另一些动画。

动画等级由高到低
Lv5.举盾
Lv4.翻滚、攻击
Lv3.跳跃
Lv2.下蹲
Lv1.跑步
Lv0.静止

实际上，一个"动作"包含不止一个动画区块，所以最好设置一些广义上"概括"的动作，
并且将各个小的细分动作和这个大动作统一起来，
也就是建立 小的细分动作名称 - 大的动作状态 - 动作优先级 的映射

实际上，处理大的动作应该是 Player 做的，
而不是 AnimSystem。

明天要想的东西:

如何保持 AnimSystem的复用性?

在 Player.cpp 中处理, 找到现在的动画阶段，并且和之前的比较
假如可以强制更新，那么更新:
   写一个函数(或者一个映射)，输入两个 PlayerAnimState 类型变量 (nowState, nextState),
   返回一个 string 代表 当前播放器应当播放哪个名字的动作
   然后 nowState = nextState

然后执行 AnimSystem 的 update
   其实就是 AnimSystem 当前管理的 AnimPlayer 的 update
   直接调用就行了

最后 再给 AnimSystem 写个方法，返回这一帧的 sf::Sprite 用于渲染
















 