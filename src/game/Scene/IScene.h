// IScene.h

// 场景统一接口,供 Director 主循环驱动
#pragma once

namespace sf {
    class Event;
    class RenderWindow;
}

// 各场景类实现本接口,暴露事件处理/更新/绘制供主循环统一调用
class IScene {
public:
    virtual ~IScene() = default;

    virtual void handleEvent(const sf::Event& event, const sf::RenderWindow& window) = 0;

    virtual void update(float deltaSeconds) = 0;

    virtual void draw(sf::RenderWindow& window) = 0;
};
