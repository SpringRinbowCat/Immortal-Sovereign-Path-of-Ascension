// IAccountObserver.h

// 账号模型的 observer 接口
#pragma once

#include "Data/AuthState.h"

// 监听鉴权状态变化的接口,由 Controller 实现
class IAccountObserver {
public:
    virtual ~IAccountObserver() = default;

    // 鉴权状态变化
    virtual void onAuthStateChanged(AuthState state) = 0;
};
