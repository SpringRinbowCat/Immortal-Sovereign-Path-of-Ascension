// ArchiveService.cpp

// 存档服务实现:后台 worker 串行执行 load/save,含原子写入、损坏备份、缺失开新档与高版本拒绝
#include "Service/Archive/ArchiveService.h"

#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iterator>
#include <mutex>
#include <queue>
#include <string>
#include <system_error>
#include <thread>
#include <utility>

#include <nlohmann/json/json.hpp>

#include "Config/Archive/ArchiveConfig.h"
#include "Data/Archive/ArchiveLoadOutcome.h"
#include "Data/PlayerArchiveData.h"
#include "Delegate/Archive/IArchiveCallback.h"
#include "Delegate/IPlatformPaths.h"
#include "Infra/Dispatcher.h"
#include "Service/Archive/ArchiveCipher.h"
#include "Service/Archive/ArchiveSerialization.h"

namespace {

// 原子写入的临时文件后缀,与正式文件同目录,rename 成功后消失
constexpr const char* kTempFileSuffix = ".tmp";

// 损坏存档备份文件的后缀前缀,后接毫秒时间戳去重
constexpr const char* kCorruptBackupPrefix = ".corrupt-";

// 二进制读整文件到 out,打开失败返回 false
bool readFileBinary(const std::filesystem::path& path, std::string& out)
{
    std::ifstream stream(path, std::ios::binary);
    if (!stream.is_open())
    {
        return false;
    }
    out.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
    return true;
}

// 以默认值构造存档数据,置入账号 ID 与当前存档版本号
PlayerArchiveData makeDefaultArchive(const std::string& accountId)
{
    PlayerArchiveData data;
    data.accountId = accountId;
    data.version = ArchiveSerialization::currentVersion();
    return data;
}

// 损坏备份文件名去重用的当前系统时间毫秒数,命名区别于其他时间工具避免冲突
long long corruptBackupTimestamp()
{
    const std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    return static_cast<long long>(ms.count());
}

}  // namespace

ArchiveService::ArchiveService(Dispatcher& dispatcher, const IPlatformPaths& paths)
    : _dispatcher(dispatcher), _paths(paths), _stopping(false)
{
    _worker = std::thread([this]() { _workerLoop(); });
}

ArchiveService::~ArchiveService()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _stopping = true;
    }
    _cv.notify_all();

    // 务必 join,等待 worker 冲刷完剩余任务再回收线程,避免析构后线程仍运行导致崩溃
    if (_worker.joinable())
    {
        _worker.join();
    }
}

void ArchiveService::load(const std::string& accountId, IArchiveCallback& callback)
{
    // 值拷贝账号 ID,指针持有 callback;封装为任务入队并唤醒 worker
    // 生命周期假设:callback 为 Controller,存活于整个场景,任务与回调执行时仍有效
    IArchiveCallback* callbackPtr = &callback;
    _enqueue([this, accountId, callbackPtr]() { _doLoad(accountId, *callbackPtr); });
}

void ArchiveService::save(const std::string& accountId, const PlayerArchiveData& data, IArchiveCallback& callback)
{
    // 值拷贝账号 ID 与存档快照,避免异步执行时源数据已变;指针持有 callback
    // 生命周期假设:callback 为 Controller,存活于整个场景,任务与回调执行时仍有效
    IArchiveCallback* callbackPtr = &callback;
    _enqueue([this, accountId, data, callbackPtr]() { _doSave(accountId, data, *callbackPtr); });
}

void ArchiveService::shutdownFlush()
{
    // 幂等/安全:已停止则 worker 可能已退出,入队屏障将永不被执行,直接返回避免死锁
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_stopping)
        {
            return;
        }
    }

    // worker 不可用时同样直接返回,不入队屏障
    if (!_worker.joinable())
    {
        return;
    }

    // 屏障任务经既有 _enqueue 入队:worker FIFO 串行,执行到它时其之前的写盘任务必已完成
    // barrier 在栈上,主线程阻塞于 wait 直至 worker set_value,故引用捕获安全
    std::promise<void> barrier;
    std::future<void> barrierDone = barrier.get_future();
    _enqueue([&barrier]() { barrier.set_value(); });

    // 同步等待屏障执行完毕,保证最后一次 save 已落盘;其后 post 的完成回调不要求再 drain
    barrierDone.wait();
}

void ArchiveService::_workerLoop()
{
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cv.wait(lock, [this]() { return _stopping || !_tasks.empty(); });

            // 停止且无剩余任务才退出;仍有任务则继续冲刷
            if (_stopping && _tasks.empty())
            {
                return;
            }

            task = std::move(_tasks.front());
            _tasks.pop();
        }

        task();  // 锁外执行,避免 I/O 期间阻塞入队
    }
}

void ArchiveService::_enqueue(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _tasks.push(std::move(task));
    }
    _cv.notify_one();
}

void ArchiveService::_doLoad(const std::string& accountId, IArchiveCallback& callback)
{
    const std::filesystem::path path = _archivePath(accountId);

    ArchiveLoadOutcome outcome;
    std::error_code existsEc;
    if (std::filesystem::exists(path, existsEc))
    {
        outcome = _loadExisting(accountId, path);
    }
    else
    {
        // 文件缺失:以默认值开新档并创建存档文件
        outcome = _createNewArchive(accountId, path);
    }

    // 回调经派发器切回主线程,worker 线程不直接调用回调
    IArchiveCallback* callbackPtr = &callback;
    _dispatcher.post([callbackPtr, outcome]() { callbackPtr->onArchiveLoaded(outcome); });
}

void ArchiveService::_doSave(const std::string& accountId, const PlayerArchiveData& data, IArchiveCallback& callback)
{
    const std::filesystem::path path = _archivePath(accountId);

    // 序列化加密后原子写入,_writeAtomic 内部按需建目录并临时文件加 rename 落盘
    const std::string cipher = ArchiveCipher::encrypt(ArchiveSerialization::toJson(data).dump());
    const bool success = _writeAtomic(path, cipher);

    // 回调经派发器切回主线程,worker 线程不直接调用回调
    IArchiveCallback* callbackPtr = &callback;
    _dispatcher.post([callbackPtr, success]() { callbackPtr->onArchiveSaved(success); });
}

std::filesystem::path ArchiveService::_archivePath(const std::string& accountId) const
{
    // string_view 常量显式转 std::string,规避 path 对 string_view 的可移植性差异
    const std::string dirName(ArchiveConfig::kArchiveDirName);
    const std::string fileName = accountId + std::string(ArchiveConfig::kArchiveFileSuffix);
    return _paths.writableDataDir() / dirName / fileName;
}

ArchiveLoadOutcome ArchiveService::_loadExisting(const std::string& accountId, const std::filesystem::path& path) const
{
    std::string cipher;
    std::string plaintext;
    nlohmann::json parsed;

    // 读取→解密→反序列化,任一步失败即视为损坏
    bool decoded = readFileBinary(path, cipher) && ArchiveCipher::decrypt(cipher, plaintext);
    if (decoded)
    {
        parsed = nlohmann::json::parse(plaintext, nullptr, false);
        decoded = !parsed.is_discarded();
    }

    if (!decoded)
    {
        // 损坏:原文件改名备份保留,不覆盖不删除,以默认值开新档
        _backupCorrupt(path);
        return ArchiveLoadOutcome{ArchiveLoadStatus::RecoveredNew, makeDefaultArchive(accountId)};
    }

    const PlayerArchiveData parsedData = ArchiveSerialization::fromJson(parsed);

    // 高版本拒绝:migrate 之前判定,拒绝加载且不修改/备份/覆盖文件
    if (ArchiveSerialization::isFutureVersion(parsedData.version))
    {
        return ArchiveLoadOutcome{ArchiveLoadStatus::RejectedTooNew, PlayerArchiveData{}};
    }

    // 归属不符拒绝:存档内账号与请求账号不一致即纯拒绝,不返回数据也不改文件
    if (parsedData.accountId != accountId)
    {
        return ArchiveLoadOutcome{ArchiveLoadStatus::RejectedWrongOwner, PlayerArchiveData{}};
    }

    return ArchiveLoadOutcome{ArchiveLoadStatus::Loaded, ArchiveSerialization::migrate(parsedData)};
}

ArchiveLoadOutcome ArchiveService::_createNewArchive(const std::string& accountId, const std::filesystem::path& path) const
{
    const PlayerArchiveData data = makeDefaultArchive(accountId);

    // 创建存档文件;写入失败不阻断,仍以默认值继续运行
    const std::string cipher = ArchiveCipher::encrypt(ArchiveSerialization::toJson(data).dump());
    _writeAtomic(path, cipher);

    return ArchiveLoadOutcome{ArchiveLoadStatus::Created, data};
}

bool ArchiveService::_writeAtomic(const std::filesystem::path& path, const std::string& cipher) const
{
    // 承接 R10.3,写入前按需创建存档目录;已存在不置错误码
    std::error_code dirEc;
    std::filesystem::create_directories(path.parent_path(), dirEc);
    if (dirEc)
    {
        return false;
    }

    std::filesystem::path tempPath = path;
    tempPath += std::string(kTempFileSuffix);  // 同目录临时文件,rename 为同一文件系统内替换

    std::ofstream stream(tempPath, std::ios::binary | std::ios::trunc);
    if (!stream.is_open())
    {
        return false;
    }
    stream.write(cipher.data(), static_cast<std::streamsize>(cipher.size()));
    stream.flush();
    stream.close();
    if (!stream)
    {
        // 写入/flush/close 任一失败,清理临时文件后返回
        std::error_code removeEc;
        std::filesystem::remove(tempPath, removeEc);
        return false;
    }

    // rename 原子替换正式文件;rename 前中断则正式文件保持上一份完整内容
    std::error_code renameEc;
    std::filesystem::rename(tempPath, path, renameEc);
    if (renameEc)
    {
        std::error_code removeEc;
        std::filesystem::remove(tempPath, removeEc);
        return false;
    }
    return true;
}

void ArchiveService::_backupCorrupt(const std::filesystem::path& path) const
{
    std::error_code existsEc;
    if (!std::filesystem::exists(path, existsEc))
    {
        return;  // 原文件已不存在,无需备份
    }

    // 备份名为原路径追加损坏后缀与毫秒时间戳,避免覆盖既有备份
    std::filesystem::path backupPath = path;
    backupPath += std::string(kCorruptBackupPrefix);
    backupPath += std::to_string(corruptBackupTimestamp());

    // 改名保留原数据,不覆盖不删除;失败仅返回不抛异常
    std::error_code renameEc;
    std::filesystem::rename(path, backupPath, renameEc);
}
