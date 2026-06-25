// ResourceConfig.h

// 资源默认参数
#pragma once

#include <string_view>

namespace ResourceConfig {
    // UI 默认字体的逻辑 key
    constexpr std::string_view kUiFontKey = "ui_default";

    // UI 默认字体相对资源目录的路径,思源黑体简体 OFL 许可,随包分发
    constexpr std::string_view kUiFontRelativePath = "fonts/SourceHanSansSC-Regular.otf";
}
