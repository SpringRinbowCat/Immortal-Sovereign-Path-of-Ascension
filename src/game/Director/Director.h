// Director.h

// 导演,持有窗口与主循环,装配场景并驱动,决定场景流转
#pragma once

#include <memory>
#include <optional>
#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Clock.hpp>

#include "Delegate/IGameFlow.h"
#include "Infra/Dispatcher.h"
#include "Infra/ResourceManager.h"
#include "Service/Archive/ArchiveService.h"

class IScene;
class IPlatformPaths;

// 拥有窗口/主循环/资源管理器,创建场景并决定流转
class Director {
public:
    Director();
    ~Director();

    void run();

private:
    // 场景标识,Director 内部概念,不暴露给 Controller
    enum class SceneId {
        Login,
        Main
    };

    // 转发场景经 IGameFlow 上报的语义事件给外层 Director,避免对外暴露流程方法
    class GameFlow : public IGameFlow {
    public:
        explicit GameFlow(Director& owner);
        void onLoginSucceeded(const std::string& accountId) override;
        void onArchiveOwnershipRejected() override;
        void onQuitRequested() override;

    private:
        Director& _owner;
    };

    // 加载 UI 字体到资源管理器,失败返回 false
    bool _loadUiFont();
    std::unique_ptr<IScene> _createScene(SceneId id);

    void _processEvents();
    void _update();
    void _render();

    // 帧末执行经 IGameFlow 记录的待切换场景或退出请求
    void _applyPendingSceneChange();

    // 按当前窗口尺寸重算等比缩放视口,保持虚拟分辨率比例并居中留边
    void _applyLetterbox();

    // 流程语义事件处理,只记录待处理请求,帧末统一执行
    void _handleLoginSucceeded(const std::string& accountId);
    void _handleArchiveOwnershipRejected();
    void _handleQuitRequested();

    sf::RenderWindow _window;
    ResourceManager _resourceManager;
    sf::Clock _frameClock;
    sf::View _uiView;
    std::unique_ptr<IPlatformPaths> _platformPaths;
    Dispatcher _dispatcher;              // 主线程派发器,须先于 _archiveService 声明与构造
    ArchiveService _archiveService;      // 存档服务,依赖 _dispatcher 与 *_platformPaths,作 IArchiveStore
    GameFlow _gameFlow;

    std::string _currentAccountId;       // 当前登录账号 ID,注入主场景作读档账号
    std::unique_ptr<IScene> _currentScene;
    std::optional<SceneId> _pendingScene;
    bool _pendingQuit;
};
