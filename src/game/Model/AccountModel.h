// AccountModel.h

// 账号模型,运行期鉴权状态与凭据校验,不依赖 SFML
#pragma once

#include <string>
#include <vector>

#include "Data/AuthState.h"
#include "Data/Login/LoginResult.h"

class IAccountObserver;

// 账号领域模型,持有登录状态并提供凭据校验
class AccountModel {
public:
    AccountModel();

    // 校验登录凭据,先账号后密码
    LoginResult checkLogin(const std::string& username, const std::string& password) const;

    // 记录账号标识并置为已登录,触发 observer 通知
    void markLoggedIn(const std::string& accountId);

    AuthState authState() const;

    const std::string& currentAccountId() const;

    // 注册者为 Controller
    void addObserver(IAccountObserver* observer);
    void removeObserver(IAccountObserver* observer);

private:
    void _notifyAuthStateChanged() const;

    AuthState _authState;
    std::string _currentAccountId;
    std::vector<IAccountObserver*> _observers;
};
