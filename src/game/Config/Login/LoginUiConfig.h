// LoginUiConfig.h

// 登录界面静态参数,不依赖 SFML
#pragma once

#include <string_view>

namespace LoginUiConfig {
    // 字号
    constexpr unsigned int kTitleCharSize = 48u;
    constexpr unsigned int kLabelCharSize = 24u;
    constexpr unsigned int kInputCharSize = 24u;
    constexpr unsigned int kButtonCharSize = 28u;
    constexpr unsigned int kErrorCharSize = 20u;
    constexpr unsigned int kDialogCharSize = 40u;

    // 布局,基于 WindowConfig 的 1280x720
    constexpr float kFieldX = 460.f;          // 账号框/密码框/按钮共用左边界
    constexpr float kFieldWidth = 360.f;
    constexpr float kFieldHeight = 48.f;
    constexpr float kAccountFieldY = 260.f;
    constexpr float kPasswordFieldY = 350.f;
    constexpr float kButtonY = 450.f;
    constexpr float kButtonHeight = 52.f;
    constexpr float kTitleY = 130.f;
    constexpr float kErrorY = 530.f;
    constexpr float kLabelGapY = 30.f;        // 标签相对输入框上方的间距
    constexpr float kTextPaddingX = 12.f;     // 文本相对输入框左内边距
    constexpr float kFieldOutlineThickness = 2.f;

    // 弹窗尺寸,居中位置由窗口尺寸推算
    constexpr float kDialogWidth = 420.f;
    constexpr float kDialogHeight = 200.f;

    // 仅接受的 ASCII 可见字符范围
    constexpr unsigned int kAsciiPrintableMin = 0x20u;
    constexpr unsigned int kAsciiPrintableMax = 0x7Eu;

    // 退格控制字符码
    constexpr unsigned int kBackspaceCode = 8u;

    // 密码掩码字符
    constexpr char kPasswordMaskChar = '*';

    // 输入光标
    constexpr float kCaretWidth = 2.f;
    constexpr float kCaretHeight = 28.f;
    constexpr float kCaretBlinkInterval = 0.5f; // 秒

    // 显示文案,UTF-8 编码
    constexpr std::string_view kTitleText = "太初灵猫录";
    constexpr std::string_view kAccountLabelText = "账号";
    constexpr std::string_view kPasswordLabelText = "密码";
    constexpr std::string_view kButtonText = "登录";
    constexpr std::string_view kErrorAccountNotFound = "账号不存在";
    constexpr std::string_view kErrorWrongPassword = "密码错误";
    constexpr std::string_view kSuccessText = "登录成功";
}
