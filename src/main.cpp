// main.cpp

// 程序入口：创建窗口并运行最小事件循环，用于验证 SFML 环境是否就绪

#include <optional>
#include <string>

#include <SFML/Graphics.hpp>

namespace
{
    // 窗口初始宽度
    const unsigned int kWindowWidth = 1280u;

    // 窗口初始高度
    const unsigned int kWindowHeight = 720u;

    // 窗口标题
    const std::string kWindowTitle = "太初灵猫录";
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({kWindowWidth, kWindowHeight}),
                            sf::String::fromUtf8(kWindowTitle.begin(), kWindowTitle.end()));

    while (window.isOpen())
    {
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();
        window.display();
    }

    return 0;
}
