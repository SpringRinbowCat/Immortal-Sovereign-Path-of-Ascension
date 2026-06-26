// MainController.h

// 主游戏控制器,接收退出上报并经 IGameFlow 转发
#pragma once

#include "Delegate/Main/IMainViewDelegate.h"

class IGameFlow;

// 主游戏场景控制器,经内部适配器接收 View 上报,避免多重继承
class MainController {
public:
    explicit MainController(IGameFlow& flow);

    // 供 Director 注册到 View 的 userEvent
    IMainViewDelegate& viewDelegate();

private:
    // 转发 View 上报给外层 Controller
    class ViewDelegate : public IMainViewDelegate {
    public:
        explicit ViewDelegate(MainController& owner);
        void onQuitRequested() override;

    private:
        MainController& _owner;
    };

    void _handleQuitRequested();

    IGameFlow& _flow;
    ViewDelegate _viewDelegate;
};
