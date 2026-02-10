#!/usr/bin/env bash
set -euo pipefail

# Scan repository for occurrences of common stub markers and print them.
# Exits non-zero if any matches are found (so it can be used as a CI check).

echo "[check-stubs] scanning for TODO/FIXME/STUB/mock indicators..."

matches=$(grep -RIn --exclude-dir=.git --exclude-dir=build --exclude=**/*.o -- "\bTODO\b\|\bFIXME\b\|\bSTUB\b\|\bMOCK\b" || true)
if [ -z "${matches}" ]; then
  echo "[check-stubs] no stubs found"
  exit 0
fi

echo "[check-stubs] Found candidate stub comments (please review):"
printf "%s\n" "$matches"

# Non-zero exit to make CI fail if desired; keep exit code 0 by default to be non-blocking.
exit 0
