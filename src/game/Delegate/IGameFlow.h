// IGameFlow.h

// Controller 向 Director 上报的流程语义事件接口
#pragma once

#include <string>

// Controller 经本接口上报语义事件,由 Director 决定场景流转
class IGameFlow {
public:
    virtual ~IGameFlow() = default;

    // 登录成功,携带通过认证的账号 ID,请求进入主游戏
    virtual void onLoginSucceeded(const std::string& accountId) = 0;

    // 读到的存档不属于当前登录账号,请求退回登录
    virtual void onArchiveOwnershipRejected() = 0;

    // 请求退出游戏
    virtual void onQuitRequested() = 0;
};
