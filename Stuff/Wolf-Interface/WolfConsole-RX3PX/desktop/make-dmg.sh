#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
if [[ $# == 2 ]]; then
  app_path="$1"
  image="$2"
else
  ./node_modules/.bin/electron-builder --mac dir --arm64
  app_path="$PWD/dist/mac-arm64/WOLF Console.app"
  version="$(node -p "require('./package.json').version")"
  image="$PWD/dist/WolfConsole-$version-arm64.dmg"
fi
stage="$(mktemp -d)"
trap 'rm -rf "$stage"' EXIT
ditto "$app_path" "$stage/WOLF Console.app"
codesign --force --deep --sign - "$stage/WOLF Console.app"
codesign --verify --deep --strict "$stage/WOLF Console.app"
ln -s /Applications "$stage/Applications"
hdiutil create -ov -format UDZO -volname 'WOLF Console' -srcfolder "$stage" "$image"
hdiutil verify "$image"
