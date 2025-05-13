#!/bin/bash
#
# Patch src/act_comm.c to fix compile warnings:
# 1. Add prototype for do_backup()
# 2. Remove unused 'name' variable
# 3. Use return-value of system() to silence warn_unused_result
#
set -e
FILE="src/act_comm.c"
BACKUP="${FILE}.bak.$(date +%Y%m%d%H%M%S)"
echo "Backing up $FILE to $BACKUP"
cp "$FILE" "$BACKUP"

# 1) Add prototype for do_backup() after includes
if ! grep -q "extern void do_backup" "$FILE"; then
  sed -i '/#include "merc.h"/a extern void do_backup(void);' "$FILE"
  echo "Inserted prototype for do_backup()"
fi

# 2) Remove unused 'name' variable declaration
sed -i '/char name\[MAX_INPUT_LENGTH\];/d' "$FILE"
echo "Removed unused variable 'name'"

# 3) Replace system(buf); with int rc=system(buf); (void)rc;
sed -i -E '
s/[[:space:]]*system\(([^)]+)\);/    int rc = system(); (void)rc;/' "$FILE"
echo "Replaced system() calls to capture return value"

echo "Patch applied to $FILE"
