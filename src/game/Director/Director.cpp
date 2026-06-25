// Director.cpp

// 导演实现
#include "Director/Director.h"

#include <iostream>
#include <optional>
#include <string>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "Config/ResourceConfig.h"
#include "Config/WindowConfig.h"
#include "Controller/Login/LoginController.h"
#include "Model/AccountModel.h"
#include "View/Login/LoginView.h"

Director::Director()
    : _window(sf::VideoMode({WindowConfig::kWindowWidth, WindowConfig::kWindowHeight}),
              sf::String::fromUtf8(WindowConfig::kWindowTitle.begin(), WindowConfig::kWindowTitle.end()))
{
}

Director::~Director() = default;

void Director::run()
{
    if (!_setupLoginScene())
    {
        std::cerr << "字体加载失败,无法启动登录场景: " << ResourceConfig::kUiFontPath << '\n';
        return;
    }

    while (_window.isOpen())
    {
        _processEvents();
        _update();
        _render();
    }
}

bool Director::_setupLoginScene()
{
    const std::string fontKey(ResourceConfig::kUiFontKey);
    if (!_resourceManager.loadFont(fontKey, std::string(ResourceConfig::kUiFontPath)))
    {
        return false;
    }

    _accountModel = std::make_unique<AccountModel>();
    _loginView = std::make_unique<LoginView>(_resourceManager.font(fontKey));
    _loginController = std::make_unique<LoginController>(*_loginView, *_accountModel);

    _loginView->setDelegate(&_loginController->viewDelegate());
    _accountModel->addObserver(&_loginController->accountObserver());
    return true;
}

void Director::_processEvents()
{
    while (const std::optional<sf::Event> event = _window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            _window.close();
        }
        else if (_loginView)
        {
            _loginView->handleEvent(*event);
        }
    }
}

void Director::_update()
{
    const float deltaSeconds = _frameClock.restart().asSeconds();
    if (_loginView)
    {
        _loginView->update(deltaSeconds);
    }
}

void Director::_render()
{
    _window.clear();
    if (_loginView)
    {
        _loginView->draw(_window);
    }
    _window.display();
}
