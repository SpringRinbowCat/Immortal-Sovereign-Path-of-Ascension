// MainController.h

// 主游戏控制器,协调读档与玩家数据,经内部适配器接收 View/Service/Model 上报
#pragma once

#include <string>

#include "Delegate/Archive/IArchiveCallback.h"
#include "Delegate/IPlayerObserver.h"
#include "Delegate/Main/IMainViewDelegate.h"

struct ArchiveLoadOutcome;
class IGameFlow;
class MainView;
class PlayerModel;
class IArchiveStore;

// 主游戏场景控制器,经内部适配器接收 View/Service/Model 上报,避免多重继承
class MainController {
public:
    MainController(MainView& view, PlayerModel& model, IArchiveStore& archiveStore, const std::string& accountId, IGameFlow& flow);

    // 供 Director 注册到 View 的 userEvent
    IMainViewDelegate& viewDelegate();

    // 供场景注册为 Service 读写完成回调
    IArchiveCallback& archiveCallback();

    // 供场景注册为 Model 的 observer
    IPlayerObserver& playerObserver();

    // 发起异步读档,须在场景完成 observer 注册后调用
    void beginLoad();

    // 每帧推进自动存计时,达间隔触发一次保存
    void update(float deltaSeconds);

    // 立即发起一次保存,供场景退出前收尾调用
    void saveNow();

private:
    // 转发 View 上报给外层 Controller
    class ViewDelegate : public IMainViewDelegate {
    public:
        explicit ViewDelegate(MainController& owner);
        void onQuitRequested() override;

    private:
        MainController& _owner;
    };

    // 转发 Service 读写完成回调给外层 Controller
    class ArchiveCallback : public IArchiveCallback {
    public:
        explicit ArchiveCallback(MainController& owner);
        void onArchiveLoaded(const ArchiveLoadOutcome& outcome) override;
        void onArchiveSaved(bool success) override;

    private:
        MainController& _owner;
    };

    // 转发 Model 通知给外层 Controller
    class PlayerObserver : public IPlayerObserver {
    public:
        explicit PlayerObserver(MainController& owner);
        void onPlayerDataChanged() override;

    private:
        MainController& _owner;
    };

    void _handleQuitRequested();
    void _handleArchiveLoaded(const ArchiveLoadOutcome& outcome);
    void _handleArchiveSaved(bool success);
    void _handlePlayerChanged();

    // 读档完成后才发起保存,置 lastSaveTimeMs 为当前系统时间
    void _saveNow();

    // 当前系统时间减上次存档时间换算秒,回拨/负值原样返回交由 settleOffline 归零
    long long _computeOfflineSeconds(long long lastSaveMs) const;

    // 当前系统时间毫秒,供离线时长与保存时间戳共用
    long long _nowMs() const;

    MainView& _view;
    PlayerModel& _model;
    IArchiveStore& _store;
    std::string _accountId;
    IGameFlow& _flow;
    ViewDelegate _viewDelegate;
    ArchiveCallback _archiveCallback;
    PlayerObserver _playerObserver;
    float _secondsSinceSave;
    bool _loaded;
};
