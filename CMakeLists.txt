# CMakeLists.txt

# 顶层构建脚本：配置 C++17、查找 SFML 并生成可执行文件

cmake_minimum_required(VERSION 3.22)

project(MaoErLunDao LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/game/Model/AccountModel.cpp
    src/game/Infra/ResourceManager.cpp
    src/game/View/Login/LoginView.cpp
    src/game/Controller/Login/LoginController.cpp
    src/game/Director/Director.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/game)

# 可执行文件输出名使用游戏英文名
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Mystic-Cat-Path-of-Ascension")

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)
