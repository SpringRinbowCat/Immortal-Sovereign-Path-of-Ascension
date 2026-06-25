// Director.h

// 导演,持有窗口与主循环,装配并驱动登录场景
#pragma once

#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Clock.hpp>

#include "Infra/ResourceManager.h"

class AccountModel;
class LoginView;
class LoginController;
class IPlatformPaths;

// 拥有窗口/主循环/资源管理器,创建对象并绑定依赖
class Director {
public:
    Director();
    ~Director();

    void run();

private:
    // 装配登录场景;字体加载失败返回 false
    bool _setupLoginScene();
    void _processEvents();
    void _update();
    void _render();

    // 按当前窗口尺寸重算等比缩放视口,保持虚拟分辨率比例并居中留边
    void _applyLetterbox();

    sf::RenderWindow _window;
    ResourceManager _resourceManager;
    sf::Clock _frameClock;
    sf::View _uiView;
    std::unique_ptr<IPlatformPaths> _platformPaths;

    std::unique_ptr<AccountModel> _accountModel;
    std::unique_ptr<LoginView> _loginView;
    std::unique_ptr<LoginController> _loginController;
};
