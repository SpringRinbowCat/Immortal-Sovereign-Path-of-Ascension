// WindowConfig.h

// 窗口静态参数
#pragma once

#include <string_view>

namespace WindowConfig {
    // 窗口初始宽度
    constexpr unsigned int kWindowWidth = 1280u;

    // 窗口初始高度
    constexpr unsigned int kWindowHeight = 720u;

    // 窗口标题,UTF-8 编码
    constexpr std::string_view kWindowTitle = "太初灵猫录";
}
