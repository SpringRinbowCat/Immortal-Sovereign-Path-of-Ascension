// AccountModel.cpp

// 账号模型实现
#include "Model/AccountModel.h"

#include <algorithm>

#include "Config/AccountConfig.h"
#include "Delegate/IAccountObserver.h"

AccountModel::AccountModel()
    : _authState(AuthState::LoggedOut)
{
}

LoginResult AccountModel::checkLogin(const std::string& username, const std::string& password) const
{
    if (username != AccountConfig::kDevAccountName)
    {
        return LoginResult::AccountNotFound;
    }

    if (password != AccountConfig::kDevAccountPassword)
    {
        return LoginResult::WrongPassword;
    }

    return LoginResult::Success;
}

void AccountModel::markLoggedIn(const std::string& accountId)
{
    _currentAccountId = accountId;
    _authState = AuthState::LoggedIn;
    _notifyAuthStateChanged();
}

AuthState AccountModel::authState() const
{
    return _authState;
}

const std::string& AccountModel::currentAccountId() const
{
    return _currentAccountId;
}

void AccountModel::addObserver(IAccountObserver* observer)
{
    if (observer == nullptr)
    {
        return;
    }

    if (std::find(_observers.begin(), _observers.end(), observer) == _observers.end())
    {
        _observers.push_back(observer);
    }
}

void AccountModel::removeObserver(IAccountObserver* observer)
{
    _observers.erase(std::remove(_observers.begin(), _observers.end(), observer), _observers.end());
}

void AccountModel::_notifyAuthStateChanged() const
{
    for (IAccountObserver* observer : _observers)
    {
        if (observer != nullptr)
        {
            observer->onAuthStateChanged(_authState);
        }
    }
}
