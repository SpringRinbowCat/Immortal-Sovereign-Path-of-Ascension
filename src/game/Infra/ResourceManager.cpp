// ResourceManager.cpp

// 资源管理器实现
#include "Infra/ResourceManager.h"

#include <filesystem>
#include <utility>

bool ResourceManager::loadFont(const std::string& key, const std::string& path)
{
    sf::Font font;
    if (!font.openFromFile(std::filesystem::path(path)))
    {
        return false;
    }

    _fonts[key] = std::move(font);
    return true;
}

bool ResourceManager::hasFont(const std::string& key) const
{
    return _fonts.find(key) != _fonts.end();
}

const sf::Font& ResourceManager::font(const std::string& key) const
{
    return _fonts.at(key);
}
