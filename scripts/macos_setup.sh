#!/usr/bin/env bash
set -euo pipefail

if [[ "$(uname -s)" != "Darwin" ]]; then
  echo "[macos_setup] This script is intended to run on macOS." >&2
  exit 1
fi

if ! xcode-select -p >/dev/null 2>&1; then
  cat >&2 <<'MSG'
Command Line Tools for Xcode are required to build ToC on macOS.
Please run `xcode-select --install` and re-run this script once the
installation completes.
MSG
  exit 1
fi

if ! command -v make >/dev/null 2>&1; then
  echo "[macos_setup] 'make' is required but was not found." >&2
  exit 1
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
LOG_DIR="$ROOT_DIR/log"
AREA_DIR="$ROOT_DIR/area"

printf '\n==> Building ToC for macOS...\n'
(
  cd "$ROOT_DIR"
  make clean
  make
)

if [[ ! -x "$ROOT_DIR/merc" ]]; then
  echo "[macos_setup] Build finished but the merc binary was not created." >&2
  exit 1
fi

mkdir -p "$LOG_DIR"
ln -sf ../merc "$AREA_DIR/merc"

cat <<'MSG'

Build complete! You can now launch the game server with:

    ./macos_launch.command

Logs will be stored in the repository's log/ directory.
MSG
