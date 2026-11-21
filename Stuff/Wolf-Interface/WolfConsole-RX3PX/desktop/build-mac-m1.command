#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
if [[ "$(uname -s)" != Darwin || "$(uname -m)" != arm64 ]]; then
  echo "Запустите на Mac M1 или новее из нативного Terminal (arm64)."; exit 1
fi
if ! command -v npm >/dev/null; then echo "Установите Node.js 22 LTS для macOS ARM64."; exit 1; fi
npm ci --no-audit --no-fund
npm run dmg
open dist
