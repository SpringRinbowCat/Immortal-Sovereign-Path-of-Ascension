// MainController.cpp

// 主游戏控制器实现
#include "Controller/Main/MainController.h"

#include <chrono>
#include <string>

#include "Config/Archive/ArchiveConfig.h"
#include "Data/Archive/ArchiveLoadOutcome.h"
#include "Data/PlayerArchiveData.h"
#include "Delegate/Archive/IArchiveStore.h"
#include "Delegate/IGameFlow.h"
#include "Model/PlayerModel.h"

namespace {
    // 毫秒换算秒的基数
    constexpr long long kMillisecondsPerSecond = 1000;
}

MainController::ViewDelegate::ViewDelegate(MainController& owner)
    : _owner(owner)
{
}

void MainController::ViewDelegate::onQuitRequested()
{
    _owner._handleQuitRequested();
}

MainController::ArchiveCallback::ArchiveCallback(MainController& owner)
    : _owner(owner)
{
}

void MainController::ArchiveCallback::onArchiveLoaded(const ArchiveLoadOutcome& outcome)
{
    _owner._handleArchiveLoaded(outcome);
}

void MainController::ArchiveCallback::onArchiveSaved(bool success)
{
    _owner._handleArchiveSaved(success);
}

MainController::PlayerObserver::PlayerObserver(MainController& owner)
    : _owner(owner)
{
}

void MainController::PlayerObserver::onPlayerDataChanged()
{
    _owner._handlePlayerChanged();
}

MainController::MainController(MainView& view, PlayerModel& model, IArchiveStore& archiveStore, const std::string& accountId, IGameFlow& flow)
    : _view(view)
    , _model(model)
    , _store(archiveStore)
    , _accountId(accountId)
    , _flow(flow)
    , _viewDelegate(*this)
    , _archiveCallback(*this)
    , _playerObserver(*this)
    , _secondsSinceSave(0.f)
    , _loaded(false)
{
}

IMainViewDelegate& MainController::viewDelegate()
{
    return _viewDelegate;
}

IArchiveCallback& MainController::archiveCallback()
{
    return _archiveCallback;
}

IPlayerObserver& MainController::playerObserver()
{
    return _playerObserver;
}

void MainController::beginLoad()
{
    _store.load(_accountId, archiveCallback());
}

void MainController::update(float deltaSeconds)
{
    _secondsSinceSave += deltaSeconds;
    if (_secondsSinceSave >= ArchiveConfig::kAutoSaveIntervalSeconds)
    {
        _secondsSinceSave = 0.f;
        _saveNow();
    }
}

void MainController::saveNow()
{
    _saveNow();
}

void MainController::_saveNow()
{
    if (!_loaded)
    {
        // 读档回调到达前不存,避免默认数据覆盖正式存档
        return;
    }

    PlayerArchiveData data = _model.toArchiveData();
    data.lastSaveTimeMs = _nowMs();
    _store.save(_accountId, data, archiveCallback());
}

void MainController::_handleQuitRequested()
{
    _flow.onQuitRequested();
}

void MainController::_handleArchiveLoaded(const ArchiveLoadOutcome& outcome)
{
    switch (outcome.status)
    {
        case ArchiveLoadStatus::Loaded:
            // 已有存档:载入后按离线时长结算收益,回拨/负值交由 settleOffline 归零
            _model.loadFrom(outcome.data);
            _model.settleOffline(_computeOfflineSeconds(outcome.data.lastSaveTimeMs));
            _loaded = true;
            break;
        case ArchiveLoadStatus::Created:
            // 全新档:仅载入不结算离线,lastSaveTimeMs 为 0 若结算会被封顶白送
            _model.loadFrom(outcome.data);
            _loaded = true;
            break;
        case ArchiveLoadStatus::RecoveredNew:
            // 损坏已备份并新建:同全新档不结算离线
            _model.loadFrom(outcome.data);
            _loaded = true;
            // TODO UI 提示"存档已损坏,已备份并新建":MainView 暂无消息展示接口,待后续 UI 任务接入
            break;
        case ArchiveLoadStatus::RejectedTooNew:
            // 版本过高:不载入数据、不置 _loaded,禁止自动存/退出存覆盖原档
            // TODO UI 提示"存档版本过高,请升级游戏":MainView 暂无消息展示接口,待后续 UI 任务接入
            break;
        case ArchiveLoadStatus::RejectedWrongOwner:
            // 归属不符退回登录,不加载数据;Director 帧末切回登录场景
            _flow.onArchiveOwnershipRejected();
            break;
    }
}

long long MainController::_computeOfflineSeconds(long long lastSaveMs) const
{
    // 回拨/负值原样返回,由 settleOffline 按 0 处理
    return (_nowMs() - lastSaveMs) / kMillisecondsPerSecond;
}

long long MainController::_nowMs() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch())
        .count();
}

void MainController::_handleArchiveSaved(bool)
{
    // 自动存/退出存完成,当前无 UI 反馈需求;失败提示待后续 UI 任务接入
}

void MainController::_handlePlayerChanged()
{
    // loadFrom/settleOffline 经此回调把玩家数据推入待显示状态
    // TODO 属性/资源 HUD 展示属后续 UI 任务;MainView 暂无属性显示接口,不引入 SFML
}
