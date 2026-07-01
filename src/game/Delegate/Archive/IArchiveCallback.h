// IArchiveCallback.h

// 存档读写完成回调接口
#pragma once

struct ArchiveLoadOutcome;

// 回调已切回主线程,唯一接收方为 Controller
class IArchiveCallback {
public:
    virtual ~IArchiveCallback() = default;

    // 读档完成,据 outcome.status 分流
    virtual void onArchiveLoaded(const ArchiveLoadOutcome& outcome) = 0;

    // 存档完成
    virtual void onArchiveSaved(bool success) = 0;
};
