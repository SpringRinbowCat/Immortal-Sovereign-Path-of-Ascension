// LoginView.cpp

// 登录界面视图实现
#include "View/Login/LoginView.h"

#include <string>
#include <string_view>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "Config/Login/LoginUiConfig.h"
#include "Config/WindowConfig.h"
#include "Data/Login/LoginRequest.h"
#include "Delegate/Login/ILoginViewDelegate.h"

namespace {
    const sf::Color kBackgroundColor(30u, 30u, 40u);
    const sf::Color kBoxFillColor(50u, 50u, 62u);
    const sf::Color kBoxOutlineColor(90u, 90u, 110u);
    const sf::Color kBoxFocusedOutlineColor(120u, 170u, 255u);
    const sf::Color kTextColor(232u, 232u, 238u);
    const sf::Color kLabelColor(198u, 198u, 210u);
    const sf::Color kButtonFillColor(70u, 120u, 200u);
    const sf::Color kButtonTextColor(255u, 255u, 255u);
    const sf::Color kErrorColor(232u, 92u, 92u);
    const sf::Color kCaretColor(232u, 232u, 238u);

    // UTF-8 文本转 sf::String
    sf::String toSfString(std::string_view text)
    {
        return sf::String::fromUtf8(text.begin(), text.end());
    }

    // 把文本水平居中到 [left, left+width] 区间,纵向置于 top
    void centerTextX(sf::Text& text, float left, float width, float top)
    {
        const sf::FloatRect bounds = text.getLocalBounds();
        const float x = left + (width - bounds.size.x) * 0.5f - bounds.position.x;
        text.setPosition({x, top});
    }
}

LoginView::LoginView(const sf::Font& font)
    : _delegate(nullptr)
    , _focusedField(InputField::None)
    , _caretVisible(true)
    , _caretBlinkTimer(0.f)
    , _titleText(font, toSfString(LoginUiConfig::kTitleText), LoginUiConfig::kTitleCharSize)
    , _accountLabel(font, toSfString(LoginUiConfig::kAccountLabelText), LoginUiConfig::kLabelCharSize)
    , _passwordLabel(font, toSfString(LoginUiConfig::kPasswordLabelText), LoginUiConfig::kLabelCharSize)
    , _accountText(font, sf::String(), LoginUiConfig::kInputCharSize)
    , _passwordText(font, sf::String(), LoginUiConfig::kInputCharSize)
    , _buttonText(font, toSfString(LoginUiConfig::kButtonText), LoginUiConfig::kButtonCharSize)
    , _errorText(font, sf::String(), LoginUiConfig::kErrorCharSize)
{
    _layoutBoxes();
    _layoutTexts();
    _refreshFocusVisual();
}

void LoginView::_layoutBoxes()
{
    _background.setSize({static_cast<float>(WindowConfig::kWindowWidth),
                         static_cast<float>(WindowConfig::kWindowHeight)});
    _background.setFillColor(kBackgroundColor);

    const sf::Vector2f fieldSize(LoginUiConfig::kFieldWidth, LoginUiConfig::kFieldHeight);
    _accountBox.setSize(fieldSize);
    _accountBox.setPosition({LoginUiConfig::kFieldX, LoginUiConfig::kAccountFieldY});
    _passwordBox.setSize(fieldSize);
    _passwordBox.setPosition({LoginUiConfig::kFieldX, LoginUiConfig::kPasswordFieldY});

    _buttonBox.setSize({LoginUiConfig::kFieldWidth, LoginUiConfig::kButtonHeight});
    _buttonBox.setPosition({LoginUiConfig::kFieldX, LoginUiConfig::kButtonY});
    _buttonBox.setFillColor(kButtonFillColor);

    _caret.setSize({LoginUiConfig::kCaretWidth, LoginUiConfig::kCaretHeight});
    _caret.setFillColor(kCaretColor);
}

void LoginView::_layoutTexts()
{
    centerTextX(_titleText, 0.f, static_cast<float>(WindowConfig::kWindowWidth), LoginUiConfig::kTitleY);

    _accountLabel.setFillColor(kLabelColor);
    _accountLabel.setPosition({LoginUiConfig::kFieldX, LoginUiConfig::kAccountFieldY - LoginUiConfig::kLabelGapY});
    _passwordLabel.setFillColor(kLabelColor);
    _passwordLabel.setPosition({LoginUiConfig::kFieldX, LoginUiConfig::kPasswordFieldY - LoginUiConfig::kLabelGapY});

    const float inputTopOffset = (LoginUiConfig::kFieldHeight - static_cast<float>(LoginUiConfig::kInputCharSize)) * 0.5f;
    _accountText.setFillColor(kTextColor);
    _accountText.setPosition({LoginUiConfig::kFieldX + LoginUiConfig::kTextPaddingX,
                              LoginUiConfig::kAccountFieldY + inputTopOffset});
    _passwordText.setFillColor(kTextColor);
    _passwordText.setPosition({LoginUiConfig::kFieldX + LoginUiConfig::kTextPaddingX,
                               LoginUiConfig::kPasswordFieldY + inputTopOffset});

    _titleText.setFillColor(kTextColor);
    _buttonText.setFillColor(kButtonTextColor);
    const float buttonTextTop = LoginUiConfig::kButtonY +
        (LoginUiConfig::kButtonHeight - static_cast<float>(LoginUiConfig::kButtonCharSize)) * 0.5f;
    centerTextX(_buttonText, LoginUiConfig::kFieldX, LoginUiConfig::kFieldWidth, buttonTextTop);

    _errorText.setFillColor(kErrorColor);
    _errorText.setPosition({LoginUiConfig::kFieldX, LoginUiConfig::kErrorY});
}

void LoginView::setDelegate(ILoginViewDelegate* delegate)
{
    _delegate = delegate;
}

void LoginView::setErrorMessage(const std::string& message)
{
    _errorText.setString(toSfString(message));
}

void LoginView::handleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    const sf::Event::MouseButtonPressed* mouse = event.getIf<sf::Event::MouseButtonPressed>();
    if (mouse != nullptr)
    {
        if (mouse->button == sf::Mouse::Button::Left)
        {
            _handleMousePressed(window.mapPixelToCoords(mouse->position));
        }
        return;
    }

    // 移动端触摸不映射为鼠标事件,需单独处理
    const sf::Event::TouchBegan* touch = event.getIf<sf::Event::TouchBegan>();
    if (touch != nullptr)
    {
        _handleMousePressed(window.mapPixelToCoords(touch->position));
        return;
    }

    const sf::Event::TextEntered* textEntered = event.getIf<sf::Event::TextEntered>();
    if (textEntered != nullptr)
    {
        _handleTextEntered(textEntered->unicode);
        return;
    }

    const sf::Event::KeyPressed* key = event.getIf<sf::Event::KeyPressed>();
    if (key != nullptr)
    {
        // 退格与回车统一由 TextEntered 处理,这里只处理桌面 Tab 切换焦点
        if (key->code == sf::Keyboard::Key::Tab)
        {
            _focusNextField();
        }
    }
}

void LoginView::update(float deltaSeconds)
{
    if (_focusedField == InputField::None)
    {
        _caretVisible = false;
        return;
    }

    _caretBlinkTimer += deltaSeconds;
    if (_caretBlinkTimer >= LoginUiConfig::kCaretBlinkInterval)
    {
        _caretBlinkTimer -= LoginUiConfig::kCaretBlinkInterval;
        _caretVisible = !_caretVisible;
    }
    _updateCaretPosition();
}

void LoginView::_updateCaretPosition()
{
    const bool isAccount = _focusedField == InputField::Account;
    const sf::Text& text = isAccount ? _accountText : _passwordText;
    const std::size_t length = isAccount ? _accountInput.size() : _passwordInput.size();
    const float fieldY = isAccount ? LoginUiConfig::kAccountFieldY : LoginUiConfig::kPasswordFieldY;

    const sf::Vector2f tail = text.findCharacterPos(length);
    const float caretTop = fieldY + (LoginUiConfig::kFieldHeight - LoginUiConfig::kCaretHeight) * 0.5f;
    _caret.setPosition({tail.x, caretTop});
}

void LoginView::draw(sf::RenderWindow& window) const
{
    window.draw(_background);
    window.draw(_titleText);
    window.draw(_accountLabel);
    window.draw(_passwordLabel);
    window.draw(_accountBox);
    window.draw(_passwordBox);
    window.draw(_accountText);
    window.draw(_passwordText);
    window.draw(_buttonBox);
    window.draw(_buttonText);
    window.draw(_errorText);

    if (_focusedField != InputField::None && _caretVisible)
    {
        window.draw(_caret);
    }
}

void LoginView::_handleMousePressed(const sf::Vector2f& position)
{
    if (_accountBox.getGlobalBounds().contains(position))
    {
        _setFocus(InputField::Account);
    }
    else if (_passwordBox.getGlobalBounds().contains(position))
    {
        _setFocus(InputField::Password);
    }
    else if (_buttonBox.getGlobalBounds().contains(position))
    {
        _submit();
    }
    else
    {
        _setFocus(InputField::None);
    }
}

void LoginView::_handleTextEntered(unsigned int unicode)
{
    if (_focusedField == InputField::None)
    {
        return;
    }

    // 退格与回车在桌面/移动端统一经 TextEntered 处理
    if (unicode == LoginUiConfig::kBackspaceCode)
    {
        _handleBackspace();
        return;
    }
    if (unicode == LoginUiConfig::kCarriageReturnCode || unicode == LoginUiConfig::kLineFeedCode)
    {
        _handleEnter();
        return;
    }

    if (unicode < LoginUiConfig::kAsciiPrintableMin || unicode > LoginUiConfig::kAsciiPrintableMax)
    {
        return;
    }

    const char character = static_cast<char>(unicode);
    if (_focusedField == InputField::Account)
    {
        _accountInput.push_back(character);
        _refreshAccountText();
    }
    else
    {
        _passwordInput.push_back(character);
        _refreshPasswordText();
    }
}

void LoginView::_handleEnter()
{
    _submit();
}

void LoginView::_handleBackspace()
{
    if (_focusedField == InputField::Account && !_accountInput.empty())
    {
        _accountInput.pop_back();
        _refreshAccountText();
    }
    else if (_focusedField == InputField::Password && !_passwordInput.empty())
    {
        _passwordInput.pop_back();
        _refreshPasswordText();
    }
}

void LoginView::_focusNextField()
{
    if (_focusedField == InputField::Account)
    {
        _setFocus(InputField::Password);
    }
    else
    {
        _setFocus(InputField::Account);
    }
}

void LoginView::_setFocus(InputField field)
{
    _focusedField = field;
    _caretVisible = true;
    _caretBlinkTimer = 0.f;
    _refreshFocusVisual();

    // 移动端聚焦输入框时唤起系统软键盘,失焦时收起;桌面端为空操作
    sf::Keyboard::setVirtualKeyboardVisible(field != InputField::None);
}

void LoginView::_refreshFocusVisual()
{
    _accountBox.setFillColor(kBoxFillColor);
    _accountBox.setOutlineThickness(LoginUiConfig::kFieldOutlineThickness);
    _passwordBox.setFillColor(kBoxFillColor);
    _passwordBox.setOutlineThickness(LoginUiConfig::kFieldOutlineThickness);

    _accountBox.setOutlineColor(_focusedField == InputField::Account ? kBoxFocusedOutlineColor : kBoxOutlineColor);
    _passwordBox.setOutlineColor(_focusedField == InputField::Password ? kBoxFocusedOutlineColor : kBoxOutlineColor);
}

void LoginView::_submit()
{
    _errorText.setString(sf::String());
    if (_delegate == nullptr)
    {
        return;
    }

    LoginRequest request;
    request.username = _accountInput;
    request.password = _passwordInput;
    _delegate->onLoginRequested(request);
}

void LoginView::_refreshAccountText()
{
    _accountText.setString(toSfString(_accountInput));
}

void LoginView::_refreshPasswordText()
{
    sf::String masked;
    for (std::size_t i = 0u; i < _passwordInput.size(); ++i)
    {
        masked += LoginUiConfig::kPasswordMaskChar;
    }
    _passwordText.setString(masked);
}
