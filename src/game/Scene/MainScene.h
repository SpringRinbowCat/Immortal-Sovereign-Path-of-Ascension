// MainScene.h

// 主游戏场景,装配并自持主游戏模块的 View/Controller
#pragma once

#include <memory>

#include "Scene/IScene.h"

class ResourceManager;
class IGameFlow;
class MainView;
class MainController;

// 主游戏场景类,实现 IScene 供 Director 驱动
class MainScene : public IScene {
public:
    MainScene(ResourceManager& resources, IGameFlow& flow);
    ~MainScene() override;

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void update(float deltaSeconds) override;
    void draw(sf::RenderWindow& window) override;

private:
    std::unique_ptr<MainController> _controller;
    std::unique_ptr<MainView> _view;
};
