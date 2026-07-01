// ArchiveSerialization.h

// PlayerArchiveData 与 JSON 的互转及存档版本迁移/高版本判定,反序列化带字段校验与回退
#pragma once

#include <nlohmann/json/json.hpp>

#include "Data/PlayerArchiveData.h"

// 存档数据与 JSON 的序列化辅助,纯标准库,不依赖 SFML
namespace ArchiveSerialization {

// 分组嵌套结构,枚举以字符串输出
nlohmann::json toJson(const PlayerArchiveData& data);

// 缺失或类型不符的字段回退默认,负数值回退 0,未知枚举回退默认;不抛异常逃逸
PlayerArchiveData fromJson(const nlohmann::json& j);

// 当前存档结构版本号,等于 ArchiveConfig::kCurrentVersion,便于测试与调用方
int currentVersion();

// 版本号高于当前客户端版本判定;调用方据此拒绝加载、不改文件
bool isFutureVersion(int version);

// 低版本迁移:为新增字段补默认并升到当前版本;等于当前版本原样返回
// 高版本不在此处理,调用方应先用 isFutureVersion 拦截并拒绝
PlayerArchiveData migrate(const PlayerArchiveData& data);

}  // namespace ArchiveSerialization
