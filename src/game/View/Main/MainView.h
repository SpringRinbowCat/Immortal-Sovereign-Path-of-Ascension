// MainView.h

// 主游戏视图,绘制退出按钮并采集点击,经 userEvent 上报
#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

class IMainViewDelegate;

// 主游戏场景视图,当前仅左下角一个退出按钮
class MainView {
public:
    explicit MainView(const sf::Font& font);

    void setDelegate(IMainViewDelegate* delegate);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);

    void draw(sf::RenderWindow& window) const;

private:
    void _handlePressed(const sf::Vector2f& position);

    IMainViewDelegate* _delegate;

    sf::RectangleShape _quitButton;
    sf::Text _quitText;
};
