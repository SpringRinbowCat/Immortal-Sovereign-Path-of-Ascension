// LoginView.h

// 登录界面视图,采集输入并被动绘制,经 userEvent 上报
#pragma once

#include <string>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

class ILoginViewDelegate;

// 登录场景视图,持有全部 SFML 绘制对象与输入框状态
class LoginView {
public:
    explicit LoginView(const sf::Font& font);

    void setDelegate(ILoginViewDelegate* delegate);

    void setErrorMessage(const std::string& message);

    void showSuccessDialog(const std::string& message);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);

    void update(float deltaSeconds);

    void draw(sf::RenderWindow& window) const;

private:
    enum class InputField {
        None,
        Account,
        Password
    };

    void _layoutBoxes();
    void _layoutTexts();

    void _handleMousePressed(const sf::Vector2f& position);
    void _handleTextEntered(unsigned int unicode);
    void _handleEnter();
    void _handleBackspace();
    void _focusNextField();

    void _setFocus(InputField field);
    void _refreshFocusVisual();
    void _updateCaretPosition();
    void _submit();
    void _refreshAccountText();
    void _refreshPasswordText();

    ILoginViewDelegate* _delegate;
    InputField _focusedField;
    bool _dialogVisible;
    bool _caretVisible;
    float _caretBlinkTimer;
    std::string _accountInput;
    std::string _passwordInput;

    sf::RectangleShape _background;
    sf::RectangleShape _accountBox;
    sf::RectangleShape _passwordBox;
    sf::RectangleShape _buttonBox;
    sf::RectangleShape _caret;
    sf::RectangleShape _dialogOverlay;
    sf::RectangleShape _dialogBox;

    sf::Text _titleText;
    sf::Text _accountLabel;
    sf::Text _passwordLabel;
    sf::Text _accountText;
    sf::Text _passwordText;
    sf::Text _buttonText;
    sf::Text _errorText;
    sf::Text _dialogText;
};
