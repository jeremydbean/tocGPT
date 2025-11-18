#!/usr/bin/env bash
set -euo pipefail

# Lightweight fuzz driver that mutates an area file and ensures the parser
# either accepts or cleanly rejects it without crashing.
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR/area"

SOURCE_AREA="toc.are"
ROUNDS=${ROUNDS:-10}
export SOURCE_AREA

if [ ! -x ../merc ]; then
  echo "merc binary is missing; build with 'make' first" >&2
  exit 1
fi

if [ ! -f "$SOURCE_AREA" ]; then
  echo "Source area $SOURCE_AREA not found" >&2
  exit 1
fi

failures=0
for i in $(seq 1 "$ROUNDS"); do
  tmpfile=$(mktemp)
  TMPFILE="$tmpfile" python - <<'PY'
import os, random, sys
src = os.environ["SOURCE_AREA"]
dst = os.environ["TMPFILE"]
with open(src, "r", errors="ignore") as f:
    data = f.read()
mutations = [
    lambda s: s.replace("#AREA", "#AREB", 1),
    lambda s: s + "\n#RANDOM %d\n$\n" % random.randint(1, 1000),
    lambda s: s.replace("#0", "#%d" % random.randint(1, 999), 1),
    lambda s: s[: max(0, len(s)//2)]
]
choice = random.choice(mutations)
with open(dst, "w") as f:
    f.write(choice(data))
PY
  if ! SOURCE_AREA="$SOURCE_AREA" TMPFILE="$tmpfile" ../merc --check-area "$tmpfile" >/tmp/merc_fuzz.log 2>&1; then
    rc=$?
    if [ "$rc" -gt 128 ]; then
      echo "Crash detected on round $i (rc=$rc)" >&2
      rm -f "$tmpfile"
      exit "$rc"
    fi
    failures=$((failures + 1))
  fi
  rm -f "$tmpfile"
done

echo "Fuzz rounds: $ROUNDS, graceful failures: $failures"
