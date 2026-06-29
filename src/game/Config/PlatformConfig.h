// PlatformConfig.h

// 平台模块静态参数
#pragma once

#include <string_view>

namespace PlatformConfig {
    // 应用数据目录文件夹名,存档/设置归于其下
    constexpr std::string_view kAppDataFolderName = "ImmortalSovereign";

    // 资源子目录名,可执行同级 assets
    constexpr std::string_view kAssetsFolderName = "assets";
}
