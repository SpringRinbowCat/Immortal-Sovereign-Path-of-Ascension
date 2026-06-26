// IMainViewDelegate.h

// 主游戏视图的 userEvent 委托接口
#pragma once

// 主游戏视图上报用户事件的接口,由 Controller 实现
class IMainViewDelegate {
public:
    virtual ~IMainViewDelegate() = default;

    // 用户请求退出游戏
    virtual void onQuitRequested() = 0;
};
