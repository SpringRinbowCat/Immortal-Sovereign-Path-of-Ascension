// PlatformPaths.cpp

// 平台路径实现,按编译期平台分支:Android / Windows / iOS / macOS
#include "Infra/Platform/PlatformPaths.h"

#include <memory>

#include "Delegate/IPlatformPaths.h"
#include "Config/PlatformConfig.h"

#if defined(__ANDROID__)

#include <SFML/System/NativeActivity.hpp>

#include <android/native_activity.h>

// Android:内部存储目录由 NativeActivity 提供;assets 在 APK 内,经 SFML 相对路径访问
class PlatformPathsImpl : public IPlatformPaths {
public:
    std::filesystem::path resourceDir() const override
    {
        // APK assets 无文件系统路径,资源以相对路径交由 SFML 的 AAssetManager 解析
        return std::filesystem::path();
    }

    std::filesystem::path writableDataDir() const override
    {
        const ANativeActivity* activity = sf::getNativeActivity();
        if (activity != nullptr && activity->internalDataPath != nullptr)
        {
            return std::filesystem::path(activity->internalDataPath);
        }
        return std::filesystem::current_path();
    }
};

#elif defined(_WIN32)

#include <cstdlib>

#include <windows.h>

namespace {
    std::filesystem::path executableDir()
    {
        wchar_t buffer[MAX_PATH];
        const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        if (length == 0u)
        {
            return std::filesystem::current_path();
        }
        return std::filesystem::path(buffer).parent_path();
    }
}

// Windows:可执行同级 assets 与 %APPDATA%
class PlatformPathsImpl : public IPlatformPaths {
public:
    std::filesystem::path resourceDir() const override
    {
        return executableDir() / std::string(PlatformConfig::kAssetsFolderName);
    }

    std::filesystem::path writableDataDir() const override
    {
        const char* appData = std::getenv("APPDATA");
        const std::filesystem::path base =
            (appData != nullptr) ? std::filesystem::path(appData) : std::filesystem::current_path();
        return base / std::string(PlatformConfig::kAppDataFolderName);
    }
};

#elif defined(__APPLE__)

#include <TargetConditionals.h>

#if TARGET_OS_IOS

#include <climits>
#include <cstdlib>

#include <CoreFoundation/CoreFoundation.h>

namespace {
    std::filesystem::path bundleResourceDir()
    {
        CFBundleRef bundle = CFBundleGetMainBundle();
        if (bundle == nullptr)
        {
            return std::filesystem::current_path();
        }

        CFURLRef url = CFBundleCopyResourcesDirectoryURL(bundle);
        if (url == nullptr)
        {
            return std::filesystem::current_path();
        }

        char buffer[PATH_MAX];
        const Boolean ok = CFURLGetFileSystemRepresentation(url, true, reinterpret_cast<UInt8*>(buffer), PATH_MAX);
        CFRelease(url);
        if (!ok)
        {
            return std::filesystem::current_path();
        }
        return std::filesystem::path(buffer);
    }
}

// iOS:Bundle 内 Resources 与沙盒 Documents
class PlatformPathsImpl : public IPlatformPaths {
public:
    std::filesystem::path resourceDir() const override
    {
        return bundleResourceDir();
    }

    std::filesystem::path writableDataDir() const override
    {
        // iOS 沙盒下 HOME 指向 App 容器
        const char* home = std::getenv("HOME");
        const std::filesystem::path base =
            (home != nullptr) ? std::filesystem::path(home) : std::filesystem::current_path();
        return base / "Documents";
    }
};

#else

#include <cstdint>
#include <cstdlib>
#include <vector>

#include <mach-o/dyld.h>

namespace {
    std::filesystem::path executableDir()
    {
        std::uint32_t size = 0u;
        _NSGetExecutablePath(nullptr, &size);

        std::vector<char> buffer(size);
        if (_NSGetExecutablePath(buffer.data(), &size) != 0)
        {
            return std::filesystem::current_path();
        }
        return std::filesystem::path(buffer.data()).parent_path();
    }
}

// macOS:App Bundle 内 Resources,开发期回退可执行同级 assets
class PlatformPathsImpl : public IPlatformPaths {
public:
    std::filesystem::path resourceDir() const override
    {
        const std::filesystem::path exeDir = executableDir();
        if (exeDir.filename() == "MacOS" && exeDir.parent_path().filename() == "Contents")
        {
            return exeDir.parent_path() / "Resources";
        }
        return exeDir / std::string(PlatformConfig::kAssetsFolderName);
    }

    std::filesystem::path writableDataDir() const override
    {
        const char* home = std::getenv("HOME");
        const std::filesystem::path base =
            (home != nullptr) ? std::filesystem::path(home) : std::filesystem::current_path();
        return base / "Library" / "Application Support" / std::string(PlatformConfig::kAppDataFolderName);
    }
};

#endif

#else

#error "PlatformPaths: 暂未支持的目标平台"

#endif

std::unique_ptr<IPlatformPaths> platform::createPlatformPaths()
{
    return std::make_unique<PlatformPathsImpl>();
}
