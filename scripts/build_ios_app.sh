#!/usr/bin/env bash

# 构建游戏的 iOS App Bundle
# 用法: build_ios_app.sh [iphonesimulator|iphoneos]   默认 iphonesimulator
set -euo pipefail

CMAKE=/opt/homebrew/bin/cmake
SDK="${1:-iphonesimulator}"
SFML_PREFIX="$(pwd)/third_party/sfml-ios-$SDK"
BUILD_DIR="build-ios-$SDK"

if [ ! -d "$SFML_PREFIX" ]; then
  echo "缺少 SFML($SDK):$SFML_PREFIX,请先运行 scripts/build_sfml_ios.sh $SDK"
  exit 1
fi

"$CMAKE" -S . -B "$BUILD_DIR" -G Ninja \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_SYSROOT="$SDK" \
  -DCMAKE_PREFIX_PATH="$SFML_PREFIX" \
  -DSFML_DIR="$SFML_PREFIX/lib/cmake/SFML" \
  -DSFML_STATIC_LIBRARIES=ON \
  -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=BOTH \
  -DCMAKE_COMPILE_WARNING_AS_ERROR=OFF

"$CMAKE" --build "$BUILD_DIR"

echo "=== App Bundle ==="
find "$BUILD_DIR" -maxdepth 2 -name "*.app"
