// MainScene.cpp

// 主游戏场景实现
#include "Scene/MainScene.h"

#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "Config/ResourceConfig.h"
#include "Controller/Main/MainController.h"
#include "Infra/ResourceManager.h"
#include "View/Main/MainView.h"

MainScene::MainScene(ResourceManager& resources, IGameFlow& flow)
    : _controller(std::make_unique<MainController>(flow))
    , _view(std::make_unique<MainView>(resources.font(std::string(ResourceConfig::kUiFontKey))))
{
    _view->setDelegate(&_controller->viewDelegate());
}

MainScene::~MainScene() = default;

void MainScene::handleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    _view->handleEvent(event, window);
}

void MainScene::update(float)
{
    // 主游戏场景当前无时间相关逻辑
}

void MainScene::draw(sf::RenderWindow& window)
{
    _view->draw(window);
}
