// IPlatformPaths.h

// 平台相关目录查询接口
#pragma once

#include <filesystem>

// 屏蔽平台差异,向上提供资源目录与可写数据目录
class IPlatformPaths {
public:
    virtual ~IPlatformPaths() = default;

    // 只读资源基准目录,字体/图片等从此解析
    virtual std::filesystem::path resourceDir() const = 0;

    // 每用户可写数据目录,存档/设置落地于此
    virtual std::filesystem::path writableDataDir() const = 0;
};
