// LoginController.h

// 登录控制器,协调校验与界面反馈
#pragma once

#include "Data/AuthState.h"
#include "Data/Login/LoginRequest.h"
#include "Delegate/IAccountObserver.h"
#include "Delegate/Login/ILoginViewDelegate.h"

class LoginView;
class AccountModel;
class IGameFlow;

// 登录场景控制器,经内部适配器接收 View 与 Model 的上报,避免多重继承
class LoginController {
public:
    LoginController(LoginView& view, AccountModel& model, IGameFlow& flow);

    // 供 Director 注册到 View 的 userEvent
    ILoginViewDelegate& viewDelegate();

    // 供 Director 注册为 Model 的 observer
    IAccountObserver& accountObserver();

private:
    // 转发 View 上报给外层 Controller
    class ViewDelegate : public ILoginViewDelegate {
    public:
        explicit ViewDelegate(LoginController& owner);
        void onLoginRequested(const LoginRequest& request) override;

    private:
        LoginController& _owner;
    };

    // 转发 Model 通知给外层 Controller
    class AccountObserver : public IAccountObserver {
    public:
        explicit AccountObserver(LoginController& owner);
        void onAuthStateChanged(AuthState state) override;

    private:
        LoginController& _owner;
    };

    void _handleLoginRequested(const LoginRequest& request);
    void _handleAuthStateChanged(AuthState state);

    LoginView& _view;
    AccountModel& _model;
    IGameFlow& _flow;
    ViewDelegate _viewDelegate;
    AccountObserver _accountObserver;
};
