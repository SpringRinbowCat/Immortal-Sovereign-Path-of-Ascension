// ProgressionConfig.h

// 属性成长与挂机速率的基准常量,占位值待数值策划确定
#pragma once

namespace ProgressionConfig {
    // 战斗属性按等级几何递推,基准为一级值,每升一级乘成长系数,肉体线与修炼小层线各自计算后相加

    // 肉体等级线的攻防血基准与成长系数
    inline constexpr double kBodyAttackBase = 10.0;
    inline constexpr double kBodyAttackGrowth = 1.10;
    inline constexpr double kBodyDefenseBase = 8.0;
    inline constexpr double kBodyDefenseGrowth = 1.10;
    inline constexpr double kBodyHealthBase = 100.0;
    inline constexpr double kBodyHealthGrowth = 1.12;

    // 修炼小层线的攻防血基准与成长系数
    inline constexpr double kMinorAttackBase = 5.0;
    inline constexpr double kMinorAttackGrowth = 1.08;
    inline constexpr double kMinorDefenseBase = 4.0;
    inline constexpr double kMinorDefenseGrowth = 1.08;
    inline constexpr double kMinorHealthBase = 50.0;
    inline constexpr double kMinorHealthGrowth = 1.10;

    // 挂机每秒产出速率基准,离线收益为速率乘以封顶后的离线秒数
    inline constexpr double kCopperCoinsPerSecond = 10.0;
    inline constexpr double kExperiencePerSecond = 5.0;
    inline constexpr double kCultivationPointsPerSecond = 2.0;
}
