// ResourceConfig.h

// 资源默认参数
#pragma once

#include <string_view>

namespace ResourceConfig {
    // UI 默认字体的逻辑 key
    constexpr std::string_view kUiFontKey = "ui_default";

    // 临时使用 macOS 系统字体以支持中文,后续改为内置 assets 字体并由 Service 按平台解析
    constexpr std::string_view kUiFontPath = "/System/Library/Fonts/Supplemental/Arial Unicode.ttf";
}
