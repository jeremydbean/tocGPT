# Agent Notes for tocGPT

- **Project**: Text-based MUD (ToC) implemented primarily in C under `src/` with additional area data files under `area/`. Build the `merc` server with `make` (uses GCC `-std=gnu89`, `-O2`, `-fcommon`, `-DROM`, and warning flags) and run `make clean` to remove objects/binary.
- **Runtime entrypoint**: `docker-entrypoint.sh` starts in `/app/area`, ensures writable dirs (`log`, `player`, `backups`, etc.), optionally launches the FastAPI web admin (controlled by `WEB_ADMIN_ENABLED`, `WEB_ADMIN_HOST`, `WEB_ADMIN_PORT`), and finally execs `./merc` on the resolved port or passes through provided commands/ports.
- **Web admin service**: `webadmin/server.py` exposes a FastAPI app with HTML UI plus JSON endpoints to append actions into `/app/area/webadmin.queue` (wizinfo broadcast, immortal command, backup, shutdown). Health endpoint checks `merc` and `uvicorn` processes; log tail endpoint reads `/app/log/toc.log`.
- **Container workflow**: README documents Docker usage—build with `docker build -t toc .` and run with `docker run ... -p 9000:9000` plus volume mounts for `player`, `backups`, and `log`. When launching the web interface, publish it on **port 9001** (replace the old 8000 convention). The server port can be overridden via `PORT`/`MUD_PORT`.
- **Code layout tips**: C headers in `src/` (`merc.h`, etc.) pair with module `.c` files for game logic (combat, skills, magic, saving/loading, etc.). The FastAPI component is Python-only and isolated in `webadmin/`.
- **Folder handling**: Do **not** modify anything under `player/` or `gods/` without explicit permission from the user.

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
