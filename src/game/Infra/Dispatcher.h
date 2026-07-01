// Dispatcher.h

// 主线程派发器,线程安全任务队列
#pragma once

#include <functional>
#include <mutex>
#include <queue>

// 引擎基础设施,任意线程投递任务,主线程每帧排空执行
class Dispatcher {
public:
    // 任意线程调用,加锁入队
    void post(std::function<void()> task);

    // 仅主线程每帧调用,整体移出队列后在锁外依次执行
    void drain();

private:
    std::mutex _mutex;
    std::queue<std::function<void()>> _tasks;
};
