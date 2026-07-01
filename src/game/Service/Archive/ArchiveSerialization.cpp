// ArchiveSerialization.cpp

// PlayerArchiveData 与 JSON 互转实现,非法字段回退默认且不抛异常逃逸
#include "Service/Archive/ArchiveSerialization.h"

#include <cstdint>
#include <string>

#include <nlohmann/json/json.hpp>

#include "Config/Archive/ArchiveConfig.h"
#include "Data/CultivationTypes.h"
#include "Data/PlayerArchiveData.h"

namespace {

// JSON 顶层键
constexpr const char* kKeyVersion = "version";
constexpr const char* kKeyAccountId = "accountId";
constexpr const char* kKeyLastSaveTimeMs = "lastSaveTimeMs";

// JSON 分组键
constexpr const char* kKeyResources = "resources";
constexpr const char* kKeyGrowth = "growth";

// resources 分组字段
constexpr const char* kKeyCopperCoins = "copperCoins";
constexpr const char* kKeyExperience = "experience";
constexpr const char* kKeyCultivationPoints = "cultivationPoints";

// growth 分组字段
constexpr const char* kKeyBodyLevel = "bodyLevel";
constexpr const char* kKeyMajorRealm = "majorRealm";
constexpr const char* kKeyMinorLevel = "minorLevel";
constexpr const char* kKeyDaoPath = "daoPath";

// MajorRealm 枚举字符串
constexpr const char* kMajorRealmQiRefining = "QiRefining";
constexpr const char* kMajorRealmFoundationBuilding = "FoundationBuilding";

// DaoPath 枚举字符串
constexpr const char* kDaoPathNone = "None";
constexpr const char* kDaoPathRenDao = "RenDao";
constexpr const char* kDaoPathTianDao = "TianDao";
constexpr const char* kDaoPathXianDao = "XianDao";
constexpr const char* kDaoPathShengDao = "ShengDao";

const char* majorRealmToString(MajorRealm realm)
{
    if (realm == MajorRealm::FoundationBuilding)
    {
        return kMajorRealmFoundationBuilding;
    }
    return kMajorRealmQiRefining;
}

MajorRealm majorRealmFromString(const std::string& text)
{
    if (text == kMajorRealmFoundationBuilding)
    {
        return MajorRealm::FoundationBuilding;
    }
    return MajorRealm::QiRefining;  // 未知或缺失回退默认
}

const char* daoPathToString(DaoPath path)
{
    switch (path)
    {
    case DaoPath::RenDao:
        return kDaoPathRenDao;
    case DaoPath::TianDao:
        return kDaoPathTianDao;
    case DaoPath::XianDao:
        return kDaoPathXianDao;
    case DaoPath::ShengDao:
        return kDaoPathShengDao;
    case DaoPath::None:
        break;
    }
    return kDaoPathNone;  // None 与未知均回退默认
}

DaoPath daoPathFromString(const std::string& text)
{
    if (text == kDaoPathRenDao)
    {
        return DaoPath::RenDao;
    }
    if (text == kDaoPathTianDao)
    {
        return DaoPath::TianDao;
    }
    if (text == kDaoPathXianDao)
    {
        return DaoPath::XianDao;
    }
    if (text == kDaoPathShengDao)
    {
        return DaoPath::ShengDao;
    }
    return DaoPath::None;  // 未知或缺失回退默认
}

std::int64_t clampNonNegative(std::int64_t value)
{
    return value < 0 ? 0 : value;
}

// 缺失或非整数回退,读到的整数原样返回
std::int64_t readInt64(const nlohmann::json& j, const char* key, std::int64_t fallback)
{
    if (!j.contains(key) || !j.at(key).is_number_integer())
    {
        return fallback;
    }
    return j.at(key).get<std::int64_t>();
}

// 缺失或非字符串回退
std::string readString(const nlohmann::json& j, const char* key, const std::string& fallback)
{
    if (!j.contains(key) || !j.at(key).is_string())
    {
        return fallback;
    }
    return j.at(key).get<std::string>();
}

// 缺失或非对象回退为空对象,便于下钻字段统一走默认
nlohmann::json readObject(const nlohmann::json& j, const char* key)
{
    if (j.contains(key) && j.at(key).is_object())
    {
        return j.at(key);
    }
    return nlohmann::json::object();
}

}  // namespace

namespace ArchiveSerialization {

nlohmann::json toJson(const PlayerArchiveData& data)
{
    nlohmann::json root;
    root[kKeyVersion] = data.version;
    root[kKeyAccountId] = data.accountId;
    root[kKeyLastSaveTimeMs] = data.lastSaveTimeMs;

    root[kKeyResources] = {
        {kKeyCopperCoins, data.copperCoins},
        {kKeyExperience, data.experience},
        {kKeyCultivationPoints, data.cultivationPoints},
    };

    root[kKeyGrowth] = {
        {kKeyBodyLevel, data.bodyLevel},
        {kKeyMajorRealm, majorRealmToString(data.majorRealm)},
        {kKeyMinorLevel, data.minorLevel},
        {kKeyDaoPath, daoPathToString(data.daoPath)},
    };

    return root;
}

PlayerArchiveData fromJson(const nlohmann::json& j)
{
    PlayerArchiveData data;  // 起步即全默认值

    if (!j.is_object())
    {
        return data;
    }

    try
    {
        data.version = static_cast<int>(readInt64(j, kKeyVersion, data.version));
        data.accountId = readString(j, kKeyAccountId, data.accountId);
        data.lastSaveTimeMs = clampNonNegative(readInt64(j, kKeyLastSaveTimeMs, data.lastSaveTimeMs));

        const nlohmann::json resources = readObject(j, kKeyResources);
        data.copperCoins = clampNonNegative(readInt64(resources, kKeyCopperCoins, data.copperCoins));
        data.experience = clampNonNegative(readInt64(resources, kKeyExperience, data.experience));
        data.cultivationPoints = clampNonNegative(readInt64(resources, kKeyCultivationPoints, data.cultivationPoints));

        const nlohmann::json growth = readObject(j, kKeyGrowth);
        data.bodyLevel = static_cast<int>(clampNonNegative(readInt64(growth, kKeyBodyLevel, data.bodyLevel)));
        data.minorLevel = static_cast<int>(clampNonNegative(readInt64(growth, kKeyMinorLevel, data.minorLevel)));
        data.majorRealm = majorRealmFromString(readString(growth, kKeyMajorRealm, std::string()));
        data.daoPath = daoPathFromString(readString(growth, kKeyDaoPath, std::string()));
    }
    catch (...)
    {
        return data;  // 异常兜底,不逃逸到调用方
    }

    return data;
}

int currentVersion()
{
    return ArchiveConfig::kCurrentVersion;
}

bool isFutureVersion(int version)
{
    return version > ArchiveConfig::kCurrentVersion;
}

PlayerArchiveData migrate(const PlayerArchiveData& data)
{
    // 高版本或等于当前版本不在此迁移:高版本由调用方先用 isFutureVersion 拦截拒绝,
    // 等于当前版本无需迁移,两者均原样返回
    if (data.version >= ArchiveConfig::kCurrentVersion)
    {
        return data;
    }

    // 低版本迁移骨架:逐版本补新增字段默认并最终升到当前版本
    // 当前 v1 尚无历史字段差异,fromJson 已对缺失字段填默认,故此处仅提升版本号
    // 后续新增版本时,在此按 migrated.version 逐级追加补该版本新增字段默认的步骤
    PlayerArchiveData migrated = data;
    migrated.version = ArchiveConfig::kCurrentVersion;
    return migrated;
}

}  // namespace ArchiveSerialization
