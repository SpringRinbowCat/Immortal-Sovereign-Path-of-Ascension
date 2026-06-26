// LoginScene.h

// 登录场景,装配并自持登录模块的 Model/View/Controller
#pragma once

#include <memory>

#include "Scene/IScene.h"

class ResourceManager;
class IGameFlow;
class AccountModel;
class LoginView;
class LoginController;

// 登录场景类,实现 IScene 供 Director 驱动
class LoginScene : public IScene {
public:
    LoginScene(ResourceManager& resources, IGameFlow& flow);
    ~LoginScene() override;

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void update(float deltaSeconds) override;
    void draw(sf::RenderWindow& window) override;

private:
    std::unique_ptr<AccountModel> _model;
    std::unique_ptr<LoginView> _view;
    std::unique_ptr<LoginController> _controller;
};
