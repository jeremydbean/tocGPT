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
