#!/usr/bin/env bash
set -euo pipefail

# Smoke-test the area boot process without opening sockets.
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR/area"

if [ ! -x ../merc ]; then
  echo "merc binary is missing; build with 'make' first" >&2
  exit 1
fi

../merc --check-areas >/tmp/merc_check.log 2>&1

echo "Area boot sanity check completed"
