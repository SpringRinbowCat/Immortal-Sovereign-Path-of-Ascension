// ILoginViewDelegate.h

// 登录视图的 userEvent 委托接口
#pragma once

#include "Data/Login/LoginRequest.h"

// 登录视图上报用户事件的接口,由 Controller 实现
class ILoginViewDelegate {
public:
    virtual ~ILoginViewDelegate() = default;

    // 用户请求登录
    virtual void onLoginRequested(const LoginRequest& request) = 0;
};
