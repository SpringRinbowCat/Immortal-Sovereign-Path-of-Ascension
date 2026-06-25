// PlatformPaths.h

// 平台路径工厂,按编译期平台返回对应实现
#pragma once

#include <memory>

class IPlatformPaths;

namespace platform {
    std::unique_ptr<IPlatformPaths> createPlatformPaths();
}
