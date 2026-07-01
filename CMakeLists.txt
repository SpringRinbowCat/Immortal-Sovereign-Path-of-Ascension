# CMakeLists.txt

# 顶层构建脚本：桌面与 iOS 分支构建,配置 C++17 并链接 SFML

cmake_minimum_required(VERSION 3.22)

project(ImmortalSovereign LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 线程库:游戏的 Infra/Dispatcher 与 Service/ArchiveService 使用 std::thread
find_package(Threads REQUIRED)

set(GAME_SOURCES
    src/main.cpp
    src/game/Model/AccountModel.cpp
    src/game/Model/PlayerModel.cpp
    src/game/Infra/ResourceManager.cpp
    src/game/Infra/Dispatcher.cpp
    src/game/Infra/Platform/PlatformPaths.cpp
    src/game/View/Login/LoginView.cpp
    src/game/View/Main/MainView.cpp
    src/game/Controller/Login/LoginController.cpp
    src/game/Controller/Main/MainController.cpp
    src/game/Service/Archive/ArchiveSerialization.cpp
    src/game/Service/Archive/ArchiveCipher.cpp
    src/game/Service/Archive/ArchiveService.cpp
    src/game/Scene/LoginScene.cpp
    src/game/Scene/MainScene.cpp
    src/game/Director/Director.cpp)

set(GAME_OUTPUT_NAME "Immortal-Sovereign-Path-of-Ascension")

if(CMAKE_SYSTEM_NAME STREQUAL "iOS")

    # iOS：使用交叉编译好的 SFML（路径经 CMAKE_PREFIX_PATH 传入），打 App Bundle
    find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Main CONFIG)

    set(UI_FONT ${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/SourceHanSansSC-Regular.otf)
    set_source_files_properties(${UI_FONT} PROPERTIES MACOSX_PACKAGE_LOCATION "fonts")

    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${GAME_SOURCES} ${UI_FONT})

    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "${GAME_OUTPUT_NAME}"
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ios/Info.plist.in
        MACOSX_BUNDLE_GUI_IDENTIFIER com.immortalsovereign.game
        MACOSX_BUNDLE_BUNDLE_NAME "ImmortalSovereign"
        MACOSX_BUNDLE_EXECUTABLE_NAME "${GAME_OUTPUT_NAME}")

    target_link_libraries(${PROJECT_NAME} PRIVATE
        SFML::Graphics SFML::Window SFML::System SFML::Main)

else()

    # 桌面（macOS / Windows / Linux）：使用系统安装的 SFML
    find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

    add_executable(${PROJECT_NAME} ${GAME_SOURCES})

    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${GAME_OUTPUT_NAME}")

    target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)

    # 把 assets 拷到可执行同级,供桌面运行期按相对路径加载资源(字体等)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)

endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

# 线程库链接:桌面与 iOS 两分支创建的都是 ${PROJECT_NAME},在此统一追加一次
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
