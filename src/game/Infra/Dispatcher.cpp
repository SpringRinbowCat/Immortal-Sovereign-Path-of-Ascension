// Dispatcher.cpp

// 主线程派发器实现
#include "Infra/Dispatcher.h"

#include <functional>
#include <mutex>
#include <queue>
#include <utility>

void Dispatcher::post(std::function<void()> task)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _tasks.push(std::move(task));
}

void Dispatcher::drain()
{
    // 先整体移出队列,再在锁外执行,避免执行期间 post 造成死锁或重入
    std::queue<std::function<void()>> pending;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _tasks.swap(pending);
    }

    while (!pending.empty())
    {
        pending.front()();
        pending.pop();
    }
}
