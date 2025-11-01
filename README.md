# Throne of Chaos (ToC) MUD

This repository contains the Throne of Chaos MUD source code and assets. The
project historically targeted Linux hosts. The current tooling keeps the Linux
experience intact while also providing a straightforward way to build and run
the server on macOS.

## macOS quick start

1. Install the Xcode Command Line Tools if they are not already available:
   ```bash
   xcode-select --install
   ```
2. Clone this repository and run the macOS set-up script from the project
   root:
   ```bash
   ./scripts/macos_setup.sh
   ```
   The script cleans any previous build artefacts, compiles the game, and
   ensures that the runtime symlink expected by the area data files is in
   place.
3. Launch the game server:
   ```bash
   ./macos_launch.command
   ```
   By default the server listens on port `9000`. Pass an alternate port as the
   first argument if desired (for example `./macos_launch.command 4000`). Log
   files are rotated automatically into the `log/` directory.

## Linux build refresher

Nothing changes for existing Linux workflows. From the repository root run:

```bash
make clean
make
```

The resulting binary is placed in the repository root and symlinked into the
`area/` directory for compatibility with historical launcher scripts.

## Reported issues

- Game crashes at login if a player file is stored with a `NULL` password.
  Workaround: reset passwords to `Kyz2D/BNiZB8Q~` (which translates to
  `toc123`).
