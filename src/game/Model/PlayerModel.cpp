// PlayerModel.cpp

// 玩家数据模型实现
#include "Model/PlayerModel.h"

#include <algorithm>
#include <cmath>

#include "Config/Archive/ArchiveConfig.h"
#include "Config/ProgressionConfig.h"
#include "Delegate/IPlayerObserver.h"

PlayerModel::PlayerModel()
    : _attack(0)
    , _defense(0)
    , _health(0)
{
}

void PlayerModel::loadFrom(const PlayerArchiveData& data)
{
    _data = data;
    _recomputeAttributes();
    _notifyChanged();
}

PlayerArchiveData PlayerModel::toArchiveData() const
{
    return _data;
}

void PlayerModel::settleOffline(long long offlineSeconds)
{
    const long long effective = _effectiveOfflineSeconds(offlineSeconds);
    _data.copperCoins += _earn(ProgressionConfig::kCopperCoinsPerSecond, effective);
    _data.experience += _earn(ProgressionConfig::kExperiencePerSecond, effective);
    _data.cultivationPoints += _earn(ProgressionConfig::kCultivationPointsPerSecond, effective);
    _notifyChanged();
}

long long PlayerModel::attack() const
{
    return _attack;
}

long long PlayerModel::defense() const
{
    return _defense;
}

long long PlayerModel::health() const
{
    return _health;
}

void PlayerModel::addObserver(IPlayerObserver* observer)
{
    if (observer == nullptr)
    {
        return;
    }

    if (std::find(_observers.begin(), _observers.end(), observer) == _observers.end())
    {
        _observers.push_back(observer);
    }
}

void PlayerModel::removeObserver(IPlayerObserver* observer)
{
    _observers.erase(std::remove(_observers.begin(), _observers.end(), observer), _observers.end());
}

void PlayerModel::_recomputeAttributes()
{
    const double attack = _lineValue(ProgressionConfig::kBodyAttackBase, ProgressionConfig::kBodyAttackGrowth, _data.bodyLevel)
        + _lineValue(ProgressionConfig::kMinorAttackBase, ProgressionConfig::kMinorAttackGrowth, _data.minorLevel)
        + _attackBonus();
    const double defense = _lineValue(ProgressionConfig::kBodyDefenseBase, ProgressionConfig::kBodyDefenseGrowth, _data.bodyLevel)
        + _lineValue(ProgressionConfig::kMinorDefenseBase, ProgressionConfig::kMinorDefenseGrowth, _data.minorLevel)
        + _defenseBonus();
    const double health = _lineValue(ProgressionConfig::kBodyHealthBase, ProgressionConfig::kBodyHealthGrowth, _data.bodyLevel)
        + _lineValue(ProgressionConfig::kMinorHealthBase, ProgressionConfig::kMinorHealthGrowth, _data.minorLevel)
        + _healthBonus();

    _attack = _toNonNegative(attack);
    _defense = _toNonNegative(defense);
    _health = _toNonNegative(health);
}

double PlayerModel::_lineValue(double base, double growth, int level) const
{
    if (level <= 0)
    {
        return 0.0;
    }

    return base * std::pow(growth, level - 1);
}

long long PlayerModel::_toNonNegative(double value) const
{
    return std::max<long long>(0, std::llround(value));
}

long long PlayerModel::_effectiveOfflineSeconds(long long offlineSeconds) const
{
    if (offlineSeconds < 0)
    {
        return 0;
    }

    return std::min(offlineSeconds, ArchiveConfig::kOfflineCapSeconds);
}

long long PlayerModel::_earn(double ratePerSecond, long long seconds) const
{
    return _toNonNegative(ratePerSecond * static_cast<double>(seconds));
}

double PlayerModel::_attackBonus() const
{
    return 0.0;
}

double PlayerModel::_defenseBonus() const
{
    return 0.0;
}

double PlayerModel::_healthBonus() const
{
    return 0.0;
}

void PlayerModel::_notifyChanged() const
{
    for (IPlayerObserver* observer : _observers)
    {
        if (observer != nullptr)
        {
            observer->onPlayerDataChanged();
        }
    }
}
