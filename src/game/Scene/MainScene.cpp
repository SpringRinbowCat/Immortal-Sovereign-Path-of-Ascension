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

MainScene::MainScene(ResourceManager& resources, IGameFlow& flow, IArchiveStore& archiveStore, const std::string& accountId)
    : _view(std::make_unique<MainView>(resources.font(std::string(ResourceConfig::kUiFontKey))))
    , _controller(std::make_unique<MainController>(*_view, _playerModel, archiveStore, accountId, flow))
{
    _view->setDelegate(&_controller->viewDelegate());
    // observer 注册须先于 beginLoad,确保读档回调触发时已能收到数据变化
    _playerModel.addObserver(&_controller->playerObserver());
    _controller->beginLoad();
}

MainScene::~MainScene()
{
    // 先摘除 observer 再让成员析构,避免 PlayerModel 悬挂指向 Controller 适配器
    _playerModel.removeObserver(&_controller->playerObserver());
}

void MainScene::handleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    _view->handleEvent(event, window);
}

void MainScene::update(float deltaSeconds)
{
    _controller->update(deltaSeconds);
}

void MainScene::draw(sf::RenderWindow& window)
{
    _view->draw(window);
}

void MainScene::onExit()
{
    // 退出前发起最终保存,Director 随后 shutdownFlush 同步落盘
    _controller->saveNow();
}
