#!/usr/bin/env bash

# 交叉编译 SFML 3.0.2 的 Android 库并安装到 third_party/sfml-android
set -euo pipefail

CMAKE=/opt/homebrew/bin/cmake
SFML_SRC=third_party/SFML
BUILD_DIR=third_party/SFML/build-android
PREFIX=third_party/sfml-android
NDK="${ANDROID_NDK_HOME:-$HOME/Library/Android/sdk/ndk/27.2.12479018}"
ABI=arm64-v8a

rm -rf "$BUILD_DIR"

"$CMAKE" -S "$SFML_SRC" -B "$BUILD_DIR" -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$NDK/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI="$ABI" \
  -DANDROID_PLATFORM=android-21 \
  -DBUILD_SHARED_LIBS=ON \
  -DCMAKE_COMPILE_WARNING_AS_ERROR=OFF \
  -DSFML_BUILD_EXAMPLES=OFF \
  -DSFML_BUILD_DOC=OFF \
  -DSFML_BUILD_TEST_SUITE=OFF \
  -DCMAKE_INSTALL_PREFIX="$PREFIX"

"$CMAKE" --build "$BUILD_DIR"
"$CMAKE" --install "$BUILD_DIR"

echo "=== Android SFML 构建完成,已安装到 $PREFIX ==="
