// ResourceManager.h

// 资源管理器,按 key 加载并缓存 SFML 字体
#pragma once

#include <string>
#include <unordered_map>

#include <SFML/Graphics/Font.hpp>

// 引擎基础设施,集中缓存字体避免重复加载
class ResourceManager {
public:
    ResourceManager() = default;

    // 加载失败返回 false 而非抛出,便于装配阶段决定降级或终止
    bool loadFont(const std::string& key, const std::string& path);

    bool hasFont(const std::string& key) const;

    // 调用前须确保该 key 已加载成功,否则抛 std::out_of_range
    const sf::Font& font(const std::string& key) const;

private:
    std::unordered_map<std::string, sf::Font> _fonts;
};
