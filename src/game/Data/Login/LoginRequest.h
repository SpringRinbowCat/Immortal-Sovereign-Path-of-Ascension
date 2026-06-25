// LoginRequest.h

// 登录请求数据
#pragma once

#include <string>

// View 上报的登录输入
struct LoginRequest {
    std::string username;
    std::string password;
};
