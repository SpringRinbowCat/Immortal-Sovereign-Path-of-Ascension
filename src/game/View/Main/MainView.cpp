// MainView.cpp

// 主游戏视图实现
#include "View/Main/MainView.h"

#include <string_view>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/String.hpp>
#include <SFML/Window/Mouse.hpp>

#include "Config/Main/MainUiConfig.h"
#include "Config/WindowConfig.h"
#include "Delegate/Main/IMainViewDelegate.h"

namespace {
    const sf::Color kQuitFillColor(120u, 60u, 60u);
    const sf::Color kQuitOutlineColor(200u, 110u, 110u);
    const sf::Color kQuitTextColor(255u, 255u, 255u);

    // UTF-8 文本转 sf::String
    sf::String toSfString(std::string_view text)
    {
        return sf::String::fromUtf8(text.begin(), text.end());
    }
}

MainView::MainView(const sf::Font& font)
    : _delegate(nullptr)
    , _quitText(font, toSfString(MainUiConfig::kQuitButtonText), MainUiConfig::kQuitButtonCharSize)
{
    const float buttonX = MainUiConfig::kQuitButtonMarginX;
    const float buttonY = static_cast<float>(WindowConfig::kWindowHeight)
        - MainUiConfig::kQuitButtonMarginY - MainUiConfig::kQuitButtonHeight;

    _quitButton.setSize({MainUiConfig::kQuitButtonWidth, MainUiConfig::kQuitButtonHeight});
    _quitButton.setPosition({buttonX, buttonY});
    _quitButton.setFillColor(kQuitFillColor);
    _quitButton.setOutlineColor(kQuitOutlineColor);
    _quitButton.setOutlineThickness(MainUiConfig::kQuitButtonOutlineThickness);

    _quitText.setFillColor(kQuitTextColor);
    const sf::FloatRect bounds = _quitText.getLocalBounds();
    const float textX = buttonX + (MainUiConfig::kQuitButtonWidth - bounds.size.x) * 0.5f - bounds.position.x;
    const float textY = buttonY +
        (MainUiConfig::kQuitButtonHeight - static_cast<float>(MainUiConfig::kQuitButtonCharSize)) * 0.5f;
    _quitText.setPosition({textX, textY});
}

void MainView::setDelegate(IMainViewDelegate* delegate)
{
    _delegate = delegate;
}

void MainView::handleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    const sf::Event::MouseButtonPressed* mouse = event.getIf<sf::Event::MouseButtonPressed>();
    if (mouse != nullptr)
    {
        if (mouse->button == sf::Mouse::Button::Left)
        {
            _handlePressed(window.mapPixelToCoords(mouse->position));
        }
        return;
    }

    // 移动端触摸不映射为鼠标事件,需单独处理
    const sf::Event::TouchBegan* touch = event.getIf<sf::Event::TouchBegan>();
    if (touch != nullptr)
    {
        _handlePressed(window.mapPixelToCoords(touch->position));
    }
}

void MainView::draw(sf::RenderWindow& window) const
{
    window.draw(_quitButton);
    window.draw(_quitText);
}

void MainView::_handlePressed(const sf::Vector2f& position)
{
    if (_delegate == nullptr)
    {
        return;
    }

    if (_quitButton.getGlobalBounds().contains(position))
    {
        _delegate->onQuitRequested();
    }
}
