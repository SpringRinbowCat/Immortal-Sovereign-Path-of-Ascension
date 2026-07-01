// MainScene.h

// 主游戏场景,装配并自持主游戏模块的 Model/View/Controller
#pragma once

#include <memory>
#include <string>

#include "Model/PlayerModel.h"
#include "Scene/IScene.h"

class ResourceManager;
class IGameFlow;
class IArchiveStore;
class MainView;
class MainController;

// 主游戏场景类,实现 IScene 供 Director 驱动
class MainScene : public IScene {
public:
    MainScene(ResourceManager& resources, IGameFlow& flow, IArchiveStore& archiveStore, const std::string& accountId);
    ~MainScene() override;

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void update(float deltaSeconds) override;
    void draw(sf::RenderWindow& window) override;
    void onExit() override;

private:
    PlayerModel _playerModel;
    std::unique_ptr<MainView> _view;
    std::unique_ptr<MainController> _controller;
};
