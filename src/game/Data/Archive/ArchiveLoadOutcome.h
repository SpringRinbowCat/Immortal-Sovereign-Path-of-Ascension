// ArchiveLoadOutcome.h

// 读档结果:状态枚举与随附存档数据,供 Controller 据状态决定提示与流程
#pragma once

#include "Data/PlayerArchiveData.h"

enum class ArchiveLoadStatus {
    Loaded,             // 正常读到
    Created,            // 缺失开新档
    RecoveredNew,       // 损坏已备份开新档
    RejectedTooNew,     // 版本过高拒绝
    RejectedWrongOwner  // 账号归属不符拒绝
};

struct ArchiveLoadOutcome {
    ArchiveLoadStatus status = ArchiveLoadStatus::Created;
    PlayerArchiveData data;   // 仅 status 为可用状态时有意义
};
