// LoginResult.h

// 登录校验结果
#pragma once

// 校验顺序:先账号后密码
enum class LoginResult {
    Success,
    AccountNotFound, // 账号不存在
    WrongPassword    // 账号存在但密码错误
};
