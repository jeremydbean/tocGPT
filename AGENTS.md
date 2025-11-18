# Agent Notes for tocGPT

- **Project**: Text-based MUD (ToC) implemented primarily in C under `src/` with additional area data files under `area/`. Build the `merc` server with `make` (uses GCC `-std=gnu89`, `-O2`, `-fcommon`, `-DROM`, and warning flags) and run `make clean` to remove objects/binary.
- **Runtime entrypoint**: `docker-entrypoint.sh` starts in `/app/area`, ensures writable dirs (`log`, `player`, `backups`, etc.), optionally launches the FastAPI web admin (controlled by `WEB_ADMIN_ENABLED`, `WEB_ADMIN_HOST`, `WEB_ADMIN_PORT`), and finally execs `./merc` on the resolved port or passes through provided commands/ports.
- **Web admin service**: `webadmin/server.py` exposes a FastAPI app with HTML UI plus JSON endpoints to append actions into `/app/area/webadmin.queue` (wizinfo broadcast, immortal command, backup, shutdown). Health endpoint checks `merc` and `uvicorn` processes; log tail endpoint reads `/app/log/toc.log`.
- **Container workflow**: README documents Docker usage—build with `docker build -t toc .` and run with `docker run ... -p 9000:9000` plus volume mounts for `player`, `backups`, and `log`. When launching the web interface, publish it on **port 9001** (replace the old 8000 convention). The server port can be overridden via `PORT`/`MUD_PORT`.
- **Code layout tips**: C headers in `src/` (`merc.h`, etc.) pair with module `.c` files for game logic (combat, skills, magic, saving/loading, etc.). The FastAPI component is Python-only and isolated in `webadmin/`.
- **Folder handling**: Do **not** modify anything under `player/` or `gods/` without explicit permission from the user.

## Money-related special cases
- **Guild clerks**: The guild join handler charges 1 coin when players use `join` with a guild clerk (see `spec_guild_clerk` in `src/special.c`).【F:src/special.c†L1399-L1468】
- **Pet shop**: The pet shop owner in rooms flagged `ROOM_PET_SHOP` sells pets for `10 * level^2` (haggling can reduce the price) and pays players for selling items; both operations move coins via `add_money`.【F:src/special.c†L1477-L1737】
- **Club entrance**: The club clerk at room vnum 80 charges a 10-coin cover to head south into the club elevator and refuses NPCs.【F:src/special.c†L1999-L2080】
- **Healer NPCs**: The healer service in `src/misc.c` bills 10–100 coins for refresh/energize casts before performing them.【F:src/misc.c†L132-L176】
- **Lore appraisals**: Using `lore` on an item deducts gold based on item level and skill (`obj->level * 20 - (chance * 2 + ch->level/2)`).【F:src/magic2.c†L83-L104】
- **Portal fees**: Certain portals (item type `ITEM_PORTAL` with `value[0] == 1`) charge 50 coins to peek and 500 coins to enter Hall of Heroes windows; insufficient funds block the action.【F:src/act_move.c†L245-L272】【F:src/act_move.c†L2897-L2934】
- **Forced guilding at level 6**: Characters who reach level 6 without a guild are auto-enrolled and lose 50 coins if they have them, otherwise their purse is emptied.【F:src/update.c†L283-L296】

## Compile warning notes
- Recent warning fixes touched `src/comm.c` (unused prompt buffer logic), `src/fight.c` (documented intentional fall-through in
  `death_cry`), `src/magic.c` (cleaned indentation and signed/unsigned comparisons; reorganized `spell_heat_metal`), and
  `src/magic2.c` (tidied `do_lore` flow, capped trap direction/keyword formatting, and aligned damage table bounds).
- `make` now completes without emitting warnings with the current toolchain flags.
- Additional `-Wshadow` cleanups: renamed shadowing locals in `act_wiz.c`, `comm.c`, `db.c`, `magic.c`, `save.c`,
  `special.c`, and `update.c` so the stricter warning set builds cleanly. Run `make WARNFLAGS='-Wall -Wextra -Wshadow'` if you
  need to spot regressions.
- Strict warning passes (`-Wsign-compare`, `-Wformat-overflow=2`) flagged real issues: `int_app` now initializes both
  `learn` and `mana_gain`, the `race_type` sentinel fills every field, and `hunt_victim` uses a bounded buffer for secret-door
  door commands.
- Enabling `-Wunused-parameter` surfaces many unused command/spell parameters in `act_comm.c`, `act_info.c`, `act_move.c`,
  `act_obj.c`, `act_wiz.c`, `comm.c`, `db.c`, `fight.c`, `interp.c`, `magic.c`, `magic2.c`, `pkill.c`, `skills.c`,
  `special.c`, `update.c`, and `hunt.c`. Most follow the standard `do_<command>(CHAR_DATA *ch, char *argument)` signature but
  ignore `argument` (or `ch/vo`) by design; add explicit `(void)` casts or minimal argument use to quiet those warnings when
  working in the affected files.
- Added `UNUSED_PARAM` in `merc.h` and applied it across `act_info.c` to silence unused-parameter warnings without suppressing
  compilation output; current builds with `-Wall -Wextra -Wshadow -Wsign-compare -Wformat-overflow=2 -Wunused-parameter`
  emit no warnings.
- Extended `UNUSED_PARAM` coverage through `act_comm.c`, `act_move.c`, `act_obj.c`, `act_wiz.c`, `comm.c`, `db.c`, `fight.c`,
  `handler.c`, `hunt.c`, `interp.c`, and the spell stubs in `magic.c`; the strict warning set now builds cleanly. To avoid
  recurring merge conflicts on this note file, append new warning summaries as standalone bullets rather than rewriting
  previous entries.
- Added explicit `UNUSED_PARAM` markers to remaining spell stubs and spec functions in `magic2.c`, `skills.c`, `special.c`,
  and `update.c` so the extended warning set builds cleanly without suppressing diagnostics.
- Including `interp.h` in the command modules and providing missing prototypes for dispel helpers and wizlist routines
  clears `-Wmissing-prototypes` diagnostics; system backup calls now check return codes instead of discarding results so
  `-Wunused-result` stays quiet under `-Wall -Wextra -Wshadow -Wsign-compare -Wformat-overflow=2 -Wunused-parameter
  -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes`.
- Running with `-Wcast-qual` surfaces a few places that cast away constness: `act_new`/`act_public` now keep arguments
  const-correct while copying mutable strings before parsing, web-admin commands duplicate the buffer before passing to
  `interpret`, `is_name` works on local copies, and `str_dup` always returns writable memory instead of the original
  const pointer. The stricter build remains warning-free under `-Wall -Wextra -Wcast-qual`.
- Addressed `-Wconversion` hotspots by using explicit size-aware allocations in `act_info.c` list builders and casting color table updates to `sh_int` in `act_comm.c`.
- Additional `-Wconversion` fixes: clamp practice/remort updates to `sh_int`, keep wimpy assignments explicit, convert bank coin math to long-sized temps, and cast telnet control bytes in `comm.c` to avoid sign-changing char initializers.
- New conversion fixes: clamp training cost deductions in `act_move.c`, cast trap effect fields and guardian hit dice to `sh_int`, switch stealing amounts to `long` with matching formats, and cast remort afflictions to the player flag width.
- Latest pass quiets additional conversion warnings: cast blindness trap effects and mount movement deductions to `sh_int` in `act_move.c`, ensure timers and poisoned drink/food effects in `act_obj.c` store through the narrower fields, and rewrite currency queries to avoid long-to-int/double promotions with integer math guarded by `INT_MAX`.
- Bit-name helpers now take `long` flag parameters to match the character flag storage, eliminating long-to-int conversion warnings in wizstat outputs and database dumps when building with the full `-Wconversion` set.
- Additional conversion cleanup in `act_wiz.c`: clamp trust, stat, resource, and object edits through a shared `clamp_sh_int` helper so wizard-set commands assign within `sh_int` bounds without triggering `-Wconversion`.
- Save-system reminders: keep `do_save`/`do_quit`/autosave aligned so players are persisted even at low levels (starting at level 1), and revisit edge cases such as disconnect timeouts, reconnect attempts while already online, level-up saves, and command-triggered saves. Future troubleshooting should focus on link-dead handling, leveling checkpoints, and ensuring critical events call `save_char_obj` without spamming logs when `CHGRP_TO` is missing.
- Save onboarding now warns first-time quitters instead of auto-saving, requires a second QUIT to delete an unsaved character, announces at level 1 that `save` is available (see `advance_level` in `src/update.c`), and auto-saves link-dead characters only if they've saved before or are above level 3.
