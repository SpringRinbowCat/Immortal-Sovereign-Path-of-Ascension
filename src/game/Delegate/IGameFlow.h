// IGameFlow.h

// Controller 向 Director 上报的流程语义事件接口
#pragma once

// Controller 经本接口上报语义事件,由 Director 决定场景流转
class IGameFlow {
public:
    virtual ~IGameFlow() = default;

    // 登录成功,请求进入主游戏
    virtual void onLoginSucceeded() = 0;

    // 请求退出游戏
    virtual void onQuitRequested() = 0;
};
