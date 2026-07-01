// PlayerArchiveData.h

// 账号存档的可序列化被动数据结构,无行为
#pragma once

#include <cstdint>
#include <string>

#include "Data/CultivationTypes.h"

struct PlayerArchiveData {
    std::string accountId;
    int version = 0;
    std::int64_t lastSaveTimeMs = 0;

    std::int64_t copperCoins = 0;       // 铜钱
    std::int64_t experience = 0;        // 经验
    std::int64_t cultivationPoints = 0; // 修为

    int bodyLevel = 0;                  // 肉体等级
    MajorRealm majorRealm = MajorRealm::QiRefining;
    int minorLevel = 0;                 // 小层数
    DaoPath daoPath = DaoPath::None;    // 所选道
};
