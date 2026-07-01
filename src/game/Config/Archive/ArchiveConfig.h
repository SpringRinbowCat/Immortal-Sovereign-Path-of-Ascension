// ArchiveConfig.h

// 存档静态参数:相对目录/文件后缀/版本号/自动存间隔/离线收益上限等具名常量
#pragma once

#include <string_view>

namespace ArchiveConfig {
    // 存档子目录名,位于平台可写数据目录下
    inline constexpr std::string_view kArchiveDirName = "saves";

    // 存档文件后缀,与账号 ID 拼接为文件名
    inline constexpr std::string_view kArchiveFileSuffix = ".sav";

    // 当前存档结构版本号,读档时校验与迁移
    inline constexpr int kCurrentVersion = 1;

    // 自动存档间隔,单位秒
    inline constexpr float kAutoSaveIntervalSeconds = 30.f;

    // 离线收益结算时长上限,单位秒,默认 12 小时
    inline constexpr long long kOfflineCapSeconds = 12LL * 60 * 60;

    // 加解密密钥不在此以明文具名常量给出,见加密方案,由任务 7.3 实现
}
