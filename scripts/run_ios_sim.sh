#!/usr/bin/env bash

# 在 iOS 模拟器上安装并启动游戏
# 用法: run_ios_sim.sh ["iPhone 17"]
set -euo pipefail

APP="build-ios-iphonesimulator/Mystic-Cat-Path-of-Ascension.app"
BUNDLE_ID="com.mysticcat.game"
DEVICE="${1:-iPhone 17}"

if [ ! -d "$APP" ]; then
  echo "缺少 App:$APP,请先运行 scripts/build_ios_app.sh iphonesimulator"
  exit 1
fi

open -a Simulator
xcrun simctl boot "$DEVICE" 2>/dev/null || true
xcrun simctl bootstatus "$DEVICE" -b || true
xcrun simctl install "$DEVICE" "$APP"
xcrun simctl launch "$DEVICE" "$BUNDLE_ID"

echo "=== 已在模拟器($DEVICE)启动 $BUNDLE_ID ==="
