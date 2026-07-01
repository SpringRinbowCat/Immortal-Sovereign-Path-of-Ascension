// IPlayerObserver.h

// 玩家数据模型的 observer 接口
#pragma once

// 玩家数据变化通知,注册者为 Controller,用于把变化推到 View
class IPlayerObserver {
public:
    virtual ~IPlayerObserver() = default;

    // 玩家数据变化
    virtual void onPlayerDataChanged() = 0;
};
