// ArchiveService.h

// IArchiveStore 本地实现:单后台 worker 串行做 JSON 文件读写与加解密,完成回调经派发器切回主线程
#pragma once

#include <condition_variable>
#include <filesystem>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "Delegate/Archive/IArchiveStore.h"

struct PlayerArchiveData;
struct ArchiveLoadOutcome;
class IArchiveCallback;
class Dispatcher;
class IPlatformPaths;

// 本地 JSON 持久化服务,后台线程串行执行 load/save 避免文件竞争,纯标准库不依赖 SFML
class ArchiveService : public IArchiveStore {
public:
    ArchiveService(Dispatcher& dispatcher, const IPlatformPaths& paths);
    ~ArchiveService();

    void load(const std::string& accountId, IArchiveCallback& callback) override;
    void save(const std::string& accountId, const PlayerArchiveData& data, IArchiveCallback& callback) override;

    // 退出前同步冲刷:阻塞主线程直到 worker 排空当前队列,保证最后一次保存落盘,非 IArchiveStore 接口
    void shutdownFlush();

private:
    // 后台线程主体,等待任务或停止,取出任务在锁外执行
    void _workerLoop();

    // 入队一个任务并唤醒 worker,任意线程调用
    void _enqueue(std::function<void()> task);

    // 在 worker 线程执行一次读档,结果经派发器回调
    void _doLoad(const std::string& accountId, IArchiveCallback& callback);

    // 在 worker 线程执行一次写档,结果经派发器回调
    void _doSave(const std::string& accountId, const PlayerArchiveData& data, IArchiveCallback& callback);

    // 平台可写目录下的存档落地路径
    std::filesystem::path _archivePath(const std::string& accountId) const;

    // 读取并解码已存在的存档文件:读/解密/解析失败→备份开新档,高版本→拒绝,否则迁移后正常加载
    ArchiveLoadOutcome _loadExisting(const std::string& accountId, const std::filesystem::path& path) const;

    // 缺失时以默认值构造存档并原子写入创建文件,回 Created
    ArchiveLoadOutcome _createNewArchive(const std::string& accountId, const std::filesystem::path& path) const;

    // 原子写入:按需建目录,先写同目录临时文件,flush/close 成功后 rename 替换,失败清理临时文件
    bool _writeAtomic(const std::filesystem::path& path, const std::string& cipher) const;

    // 损坏存档改名备份为带时间戳的副本,不覆盖不删除原数据
    void _backupCorrupt(const std::filesystem::path& path) const;

    Dispatcher& _dispatcher;
    const IPlatformPaths& _paths;
    std::thread _worker;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::queue<std::function<void()>> _tasks;
    bool _stopping;
};
