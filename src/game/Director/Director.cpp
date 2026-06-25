// Director.cpp

// 导演实现
#include "Director/Director.h"

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "Config/ResourceConfig.h"
#include "Config/WindowConfig.h"
#include "Controller/Login/LoginController.h"
#include "Delegate/IPlatformPaths.h"
#include "Infra/Platform/PlatformPaths.h"
#include "Model/AccountModel.h"
#include "View/Login/LoginView.h"

namespace {
    // 计算保持虚拟分辨率比例的归一化视口,多余区域居中留黑边
    sf::FloatRect letterboxViewport(sf::Vector2u windowSize, float virtualWidth, float virtualHeight)
    {
        if (windowSize.x == 0u || windowSize.y == 0u)
        {
            return sf::FloatRect({0.f, 0.f}, {1.f, 1.f});
        }

        const float windowRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
        const float viewRatio = virtualWidth / virtualHeight;

        float sizeX = 1.f;
        float sizeY = 1.f;
        float posX = 0.f;
        float posY = 0.f;
        if (windowRatio >= viewRatio)
        {
            sizeX = viewRatio / windowRatio;
            posX = (1.f - sizeX) * 0.5f;
        }
        else
        {
            sizeY = windowRatio / viewRatio;
            posY = (1.f - sizeY) * 0.5f;
        }
        return sf::FloatRect({posX, posY}, {sizeX, sizeY});
    }
}

Director::Director()
    : _window(sf::VideoMode({WindowConfig::kWindowWidth, WindowConfig::kWindowHeight}),
              sf::String::fromUtf8(WindowConfig::kWindowTitle.begin(), WindowConfig::kWindowTitle.end()))
    , _uiView(sf::FloatRect({0.f, 0.f},
                            {static_cast<float>(WindowConfig::kWindowWidth),
                             static_cast<float>(WindowConfig::kWindowHeight)}))
    , _platformPaths(platform::createPlatformPaths())
{
    _applyLetterbox();
}

Director::~Director() = default;

void Director::run()
{
    if (!_setupLoginScene())
    {
        std::cerr << "登录场景装配失败,程序退出\n";
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
    const std::filesystem::path fontPath =
        _platformPaths->resourceDir() / std::string(ResourceConfig::kUiFontRelativePath);
    if (!_resourceManager.loadFont(fontKey, fontPath.string()))
    {
        std::cerr << "字体加载失败: " << fontPath << '\n';
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
        else if (event->is<sf::Event::Resized>())
        {
            _applyLetterbox();
        }
        else if (_loginView)
        {
            _loginView->handleEvent(*event, _window);
        }
    }
}

void Director::_applyLetterbox()
{
    const float virtualWidth = static_cast<float>(WindowConfig::kWindowWidth);
    const float virtualHeight = static_cast<float>(WindowConfig::kWindowHeight);
    _uiView.setViewport(letterboxViewport(_window.getSize(), virtualWidth, virtualHeight));
    _window.setView(_uiView);
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
