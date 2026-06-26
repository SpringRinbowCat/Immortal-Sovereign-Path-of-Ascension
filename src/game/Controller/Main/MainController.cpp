// MainController.cpp

// 主游戏控制器实现
#include "Controller/Main/MainController.h"

#include "Delegate/IGameFlow.h"

MainController::ViewDelegate::ViewDelegate(MainController& owner)
    : _owner(owner)
{
}

void MainController::ViewDelegate::onQuitRequested()
{
    _owner._handleQuitRequested();
}

MainController::MainController(IGameFlow& flow)
    : _flow(flow)
    , _viewDelegate(*this)
{
}

IMainViewDelegate& MainController::viewDelegate()
{
    return _viewDelegate;
}

void MainController::_handleQuitRequested()
{
    _flow.onQuitRequested();
}
