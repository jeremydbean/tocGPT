#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
AREA_DIR="$ROOT_DIR/area"
BIN="$ROOT_DIR/merc"
PORT="${1:-9000}"
LOG_DIR="$ROOT_DIR/log"
TIMESTAMP="$(date +%Y%m%d-%H%M%S)"
LOG_FILE="$LOG_DIR/macos-run-$TIMESTAMP.log"

if [[ ! -x "$BIN" ]]; then
  cat >&2 <<'MSG'
The ToC server binary (merc) was not found or is not executable.
Run ./scripts/macos_setup.sh first to build the project.
MSG
  exit 1
fi

mkdir -p "$LOG_DIR"

pushd "$AREA_DIR" >/dev/null
trap 'popd >/dev/null' EXIT

echo "Launching ToC on port $PORT"
echo "Logging to $LOG_FILE"

../merc "$PORT" | tee "$LOG_FILE"
