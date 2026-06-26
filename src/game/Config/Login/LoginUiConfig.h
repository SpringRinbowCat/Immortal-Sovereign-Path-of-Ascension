// LoginUiConfig.h

// 登录界面静态参数,不依赖 SFML
#pragma once

#include <string_view>

namespace LoginUiConfig {
    // 字号
    constexpr unsigned int kTitleCharSize = 64u;
    constexpr unsigned int kLabelCharSize = 30u;
    constexpr unsigned int kInputCharSize = 32u;
    constexpr unsigned int kButtonCharSize = 38u;
    constexpr unsigned int kErrorCharSize = 26u;

    // 布局,基于 WindowConfig 的竖屏 720x1280
    constexpr float kFieldX = 80.f;           // 账号框/密码框/按钮共用左边界
    constexpr float kFieldWidth = 560.f;
    constexpr float kFieldHeight = 64.f;
    constexpr float kAccountFieldY = 470.f;
    constexpr float kPasswordFieldY = 600.f;
    constexpr float kButtonY = 760.f;
    constexpr float kButtonHeight = 72.f;
    constexpr float kTitleY = 220.f;
    constexpr float kErrorY = 870.f;
    constexpr float kLabelGapY = 38.f;        // 标签相对输入框上方的间距
    constexpr float kTextPaddingX = 16.f;     // 文本相对输入框左内边距
    constexpr float kFieldOutlineThickness = 2.f;

    // 仅接受的 ASCII 可见字符范围
    constexpr unsigned int kAsciiPrintableMin = 0x20u;
    constexpr unsigned int kAsciiPrintableMax = 0x7Eu;

    // 退格控制字符码
    constexpr unsigned int kBackspaceCode = 8u;

    // 回车控制字符码,回车键提交登录
    constexpr unsigned int kCarriageReturnCode = 13u;
    constexpr unsigned int kLineFeedCode = 10u;

    // 密码掩码字符
    constexpr char kPasswordMaskChar = '*';

    // 输入光标
    constexpr float kCaretWidth = 2.f;
    constexpr float kCaretHeight = 40.f;
    constexpr float kCaretBlinkInterval = 0.5f; // 秒

    // 显示文案,UTF-8 编码
    constexpr std::string_view kTitleText = "太初灵猫录";
    constexpr std::string_view kAccountLabelText = "账号";
    constexpr std::string_view kPasswordLabelText = "密码";
    constexpr std::string_view kButtonText = "登录";
    constexpr std::string_view kErrorAccountNotFound = "账号不存在";
    constexpr std::string_view kErrorWrongPassword = "密码错误";
}
