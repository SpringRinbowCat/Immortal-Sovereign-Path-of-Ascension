// LoginScene.cpp

// 登录场景实现
#include "Scene/LoginScene.h"

#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "Config/ResourceConfig.h"
#include "Controller/Login/LoginController.h"
#include "Infra/ResourceManager.h"
#include "Model/AccountModel.h"
#include "View/Login/LoginView.h"

LoginScene::LoginScene(ResourceManager& resources, IGameFlow& flow)
    : _model(std::make_unique<AccountModel>())
    , _view(std::make_unique<LoginView>(resources.font(std::string(ResourceConfig::kUiFontKey))))
    , _controller(std::make_unique<LoginController>(*_view, *_model, flow))
{
    _view->setDelegate(&_controller->viewDelegate());
    _model->addObserver(&_controller->accountObserver());
}

LoginScene::~LoginScene() = default;

void LoginScene::handleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    _view->handleEvent(event, window);
}

void LoginScene::update(float deltaSeconds)
{
    _view->update(deltaSeconds);
}

void LoginScene::draw(sf::RenderWindow& window)
{
    _view->draw(window);
}
