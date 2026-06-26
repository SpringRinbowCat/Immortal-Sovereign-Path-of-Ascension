// LoginController.cpp

// 登录控制器实现
#include "Controller/Login/LoginController.h"

#include <string>

#include "Config/AccountConfig.h"
#include "Config/Login/LoginUiConfig.h"
#include "Data/Login/LoginResult.h"
#include "Delegate/IGameFlow.h"
#include "Model/AccountModel.h"
#include "View/Login/LoginView.h"

LoginController::ViewDelegate::ViewDelegate(LoginController& owner)
    : _owner(owner)
{
}

void LoginController::ViewDelegate::onLoginRequested(const LoginRequest& request)
{
    _owner._handleLoginRequested(request);
}

LoginController::AccountObserver::AccountObserver(LoginController& owner)
    : _owner(owner)
{
}

void LoginController::AccountObserver::onAuthStateChanged(AuthState state)
{
    _owner._handleAuthStateChanged(state);
}

LoginController::LoginController(LoginView& view, AccountModel& model, IGameFlow& flow)
    : _view(view)
    , _model(model)
    , _flow(flow)
    , _viewDelegate(*this)
    , _accountObserver(*this)
{
}

ILoginViewDelegate& LoginController::viewDelegate()
{
    return _viewDelegate;
}

IAccountObserver& LoginController::accountObserver()
{
    return _accountObserver;
}

void LoginController::_handleLoginRequested(const LoginRequest& request)
{
    const LoginResult result = _model.checkLogin(request.username, request.password);
    switch (result)
    {
        case LoginResult::Success:
            _model.markLoggedIn(std::string(AccountConfig::kLocalAccountId));
            break;
        case LoginResult::AccountNotFound:
            _view.setErrorMessage(std::string(LoginUiConfig::kErrorAccountNotFound));
            break;
        case LoginResult::WrongPassword:
            _view.setErrorMessage(std::string(LoginUiConfig::kErrorWrongPassword));
            break;
    }
}

void LoginController::_handleAuthStateChanged(AuthState state)
{
    if (state == AuthState::LoggedIn)
    {
        _flow.onLoginSucceeded();
    }
}
