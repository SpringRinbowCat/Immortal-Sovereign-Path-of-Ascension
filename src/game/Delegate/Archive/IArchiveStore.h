// IArchiveStore.h

// 账号存档持久化抽象接口
#pragma once

#include <string>

struct PlayerArchiveData;
class IArchiveCallback;

// 异步 load/save 账号存档,结果经回调切回主线程返回,由 Service 层实现
class IArchiveStore {
public:
    virtual ~IArchiveStore() = default;

    // 异步加载,完成经回调通知
    virtual void load(const std::string& accountId, IArchiveCallback& callback) = 0;

    // 异步保存,完成经回调通知
    virtual void save(const std::string& accountId, const PlayerArchiveData& data, IArchiveCallback& callback) = 0;
};
