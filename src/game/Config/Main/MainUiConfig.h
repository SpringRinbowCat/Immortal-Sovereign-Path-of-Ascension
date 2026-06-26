// MainUiConfig.h

// 主游戏界面静态参数,不依赖 SFML
#pragma once

#include <string_view>

namespace MainUiConfig {
    // 退出按钮尺寸
    constexpr float kQuitButtonWidth = 220.f;
    constexpr float kQuitButtonHeight = 72.f;

    // 退出按钮相对窗口左下角的边距
    constexpr float kQuitButtonMarginX = 40.f;
    constexpr float kQuitButtonMarginY = 40.f;

    constexpr unsigned int kQuitButtonCharSize = 34u;
    constexpr float kQuitButtonOutlineThickness = 2.f;

    // 显示文案,UTF-8 编码
    constexpr std::string_view kQuitButtonText = "退出游戏";
}
