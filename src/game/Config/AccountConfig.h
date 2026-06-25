// AccountConfig.h

// 账号模块静态参数
#pragma once

#include <string_view>

namespace AccountConfig {
    // 临时开发占位凭据,无安全性,接入服务器鉴权后删除
    constexpr std::string_view kDevAccountName = "admin";
    constexpr std::string_view kDevAccountPassword = "admin";

    // 本地阶段账号标识占位,接入服务器账号体系后由后端下发
    constexpr std::string_view kLocalAccountId = "local-admin";
}
