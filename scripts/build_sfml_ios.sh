#!/usr/bin/env bash

# 交叉编译 SFML 3.0.2 的 iOS 静态库
# 用法: build_sfml_ios.sh [iphoneos|iphonesimulator]   默认 iphoneos
set -euo pipefail

SDK="${1:-iphoneos}"
CMAKE=/opt/homebrew/bin/cmake
SFML_SRC=third_party/SFML
BUILD_DIR="third_party/SFML/build-ios-$SDK"
PREFIX="third_party/sfml-ios-$SDK"

rm -rf "$BUILD_DIR"

"$CMAKE" -S "$SFML_SRC" -B "$BUILD_DIR" -G Ninja \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_SYSROOT="$SDK" \
  -DBUILD_SHARED_LIBS=OFF \
  -DCMAKE_COMPILE_WARNING_AS_ERROR=OFF \
  -DSFML_BUILD_EXAMPLES=OFF \
  -DSFML_BUILD_DOC=OFF \
  -DSFML_BUILD_TEST_SUITE=OFF \
  -DCMAKE_INSTALL_PREFIX="$PREFIX"

"$CMAKE" --build "$BUILD_DIR"
"$CMAKE" --install "$BUILD_DIR"

echo "=== iOS($SDK) SFML 构建完成,已安装到 $PREFIX ==="
