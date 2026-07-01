// PlayerModel.h

// 玩家领域模型,运行期数据真相,按公式现算战斗属性并经 observer 通知 Controller
#pragma once

#include <vector>

#include "Data/PlayerArchiveData.h"

class IPlayerObserver;

// 持资源与成长状态,按肉体线与小层线公式现算攻防血,数据变化经 observer 通知
class PlayerModel {
public:
    PlayerModel();

    // 用读到的存档初始化,现算属性并通知 observer
    void loadFrom(const PlayerArchiveData& data);

    PlayerArchiveData toArchiveData() const;

    // 结算离线收益,有效时长回拨按 0 且封顶,按速率累加铜钱/经验/修为
    void settleOffline(long long offlineSeconds);

    long long attack() const;
    long long defense() const;
    long long health() const;

    // 注册者为 Controller
    void addObserver(IPlayerObserver* observer);
    void removeObserver(IPlayerObserver* observer);

private:
    // 攻防血 = 肉体线 + 小层线 + 加成
    void _recomputeAttributes();
    // 单条成长线几何递推,等级为 0 时该线贡献 0
    double _lineValue(double base, double growth, int level) const;
    // 四舍五入并夹为非负
    long long _toNonNegative(double value) const;
    // 有效离线时长,回拨按 0,超上限按上限
    long long _effectiveOfflineSeconds(long long offlineSeconds) const;
    // 按每秒速率折算离线收益,四舍五入且非负
    long long _earn(double ratePerSecond, long long seconds) const;
    // 加成预留,装备/词条接入后并入,当前均为 0
    double _attackBonus() const;
    double _defenseBonus() const;
    double _healthBonus() const;
    void _notifyChanged() const;

    PlayerArchiveData _data;
    long long _attack;
    long long _defense;
    long long _health;
    std::vector<IPlayerObserver*> _observers;
};
