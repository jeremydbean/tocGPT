
## Docker usage
You can run the MUD inside a portable Docker container. The steps below start with getting Docker on each major OS, then show how to copy this code locally and run the container from a terminal. GUI download options are noted for users who prefer them.

### 1) Install Docker

#### macOS
- **Terminal-first (recommended):**
  1. Install [Homebrew](https://brew.sh/) if you do not already have it.
  2. In Terminal, install Docker Desktop:  
     ```bash
     brew install --cask docker
     ```
  3. Open Docker Desktop from Launchpad or run `open /Applications/Docker.app` so it can finish setup.
- **GUI option:** Download Docker Desktop for Mac from <https://www.docker.com/products/docker-desktop>, drag it to Applications, then launch it once to complete initialization.

#### Windows 10/11
- **PowerShell-first (recommended):**
  1. Enable virtualization (most systems ship enabled; otherwise turn on “Virtualization Technology” in BIOS/UEFI).
  2. Install WSL 2 and the kernel update (if not already installed):  
     ```powershell
     wsl --install
     ```
  3. Install Docker Desktop via winget:  
     ```powershell
     winget install -e --id Docker.DockerDesktop
     ```
  4. Start Docker Desktop once; accept prompts to enable WSL integration.
- **GUI option:** Download Docker Desktop for Windows from <https://www.docker.com/products/docker-desktop>, run the installer, enable WSL 2 integration when prompted, and launch Docker Desktop.

#### Ubuntu (and Debian-based Linux)
- **Terminal (apt):**
  ```bash
  sudo apt update
  sudo apt install -y docker.io
  sudo systemctl enable --now docker
  sudo usermod -aG docker "$USER"
  ```
  Log out/in or run `newgrp docker` so your user can run Docker without sudo.
- **Other distributions:** Install the vendor Docker Engine package for your distro (for example, `dnf install docker` on Fedora) or download static binaries from <https://docs.docker.com/engine/install/>.

### 2) Install Git
If Git is not already available, install it using the package manager for your platform.

- **macOS (Homebrew):**
  ```bash
  brew install git
  ```
- **Windows:** Download and install [Git for Windows](https://git-scm.com/download/win), then use the bundled Git Bash or enable "Git from the command line" during setup.
- **Ubuntu / Debian:**
  ```bash
  sudo apt update
  sudo apt install -y git
  ```
- **Other Linux distributions:** Install the `git` package via your distro's package manager (for example, `dnf install git` on Fedora).

### 3) Copy the code locally
Use Git to clone the repository into a working folder.

- **macOS / Linux (bash/zsh):**
  ```bash
  git clone https://github.com/jeremydbean/tocGPT.git
  cd tocGPT
  ```

- **Windows PowerShell:**
  ```powershell
  git clone https://github.com/jeremydbean/tocGPT.git
  cd tocGPT
  ```

### 4) Build and run the Docker container
Run these commands from inside the cloned `tocGPT` folder. Substitute `${PWD}` for `$(pwd)` when using PowerShell.

1. Build the image:
   ```bash
   docker build -t toc .
   ```
2. Start the game server on the default port 9000 with host persistence (recommended):
   ```bash
   docker run --rm -it \
     -p 9000:9000 \
     -v $(pwd)/player:/app/player \
     -v $(pwd)/backups:/app/backups \
     -v $(pwd)/log:/app/log \
     toc
   ```
3. Change the port (optional) by setting `PORT`/`MUD_PORT` while keeping persistence:
   ```bash
   docker run --rm -it \
     -e PORT=4000 \
     -p 4000:4000 \
     -v $(pwd)/player:/app/player \
     -v $(pwd)/backups:/app/backups \
     -v $(pwd)/log:/app/log \
     toc
   ```
4. Publish the web admin dashboard (port 9001) alongside the game port (persistent storage). The dashboard binds to `127.0.0.1` by default inside the container; change `WEB_ADMIN_HOST` to `0.0.0.0` only if you need to expose it beyond the host:
   ```bash
   docker run --rm -it \
     -p 9000:9000 \   # game
     -p 9001:9001 \   # web admin
     -v $(pwd)/player:/app/player \
     -v $(pwd)/backups:/app/backups \
     -v $(pwd)/log:/app/log \
     toc
   ```
5. Run without host persistence (not recommended): drop the `-v` flags to use container-local storage only:
   ```bash
   docker run --rm -it -p 9000:9000 toc
   ```

The container entrypoint accepts optional arguments if you need to pass flags directly to the `merc` binary (for example, `newlock` to block new players). If no arguments are provided, it automatically starts the server on the configured port.

## Web administration dashboard
The container starts a lightweight FastAPI web dashboard alongside the game server. It provides a browser UI plus JSON endpoints so admins can queue in-game actions without logging in as an immortal. By default it listens on port `9001` inside the container and can be disabled with `WEB_ADMIN_ENABLED=0`.

### How to expose and open the dashboard
1. Publish the admin port when launching the container (change host ports as desired). By default the service binds to `127.0.0.1:9001` inside the container; use `-e WEB_ADMIN_HOST=0.0.0.0` only if you must expose it outside the host:
   ```bash
   docker run --rm -it \
     -p 9000:9000 \   # game
     -p 9001:9001 \   # web admin
     -v $(pwd)/player:/app/player \
     -v $(pwd)/backups:/app/backups \
     -v $(pwd)/log:/app/log \
     toc
   ```
2. Open <http://localhost:9001/> in your browser to use the UI.

### Available actions (UI and API)
- **WizInfo broadcast:** Sends a WizInfo message to connected players at the chosen minimum level (defaults to 62). Queued as `wizinfo|<level>|<message>` in `/app/area/webadmin.queue`.
- **Run immortal command:** Queues `command|<text>` which the game executes through the highest-trust connected immortal. Use this for admin-only commands like `copyover`, `wizinvis`, or moderation tools.
- **Backup:** Queues `backup`, which invokes the existing `do_backup` routine from inside the game loop.
- **Shutdown:** Queues `shutdown` for a clean server stop.
- **Log tail:** Shows the last 200 lines of `log/toc.log` and refreshes every 15 seconds. Also available at `/api/logs`.
- **Health:** `/api/health` returns process flags for the MUD (`merc`) and the web service itself.

### Endpoint summary
| Method | Path            | Body                           | Purpose                             |
|--------|-----------------|--------------------------------|-------------------------------------|
| GET    | `/`             | —                              | HTML dashboard                      |
| GET    | `/api/health`   | —                              | Process status JSON                 |
| GET    | `/api/logs`     | `?lines=200` (optional)        | Last N lines of `log/toc.log`       |
| POST   | `/api/wizinfo`  | `{ "message": "text", "level": 62? }` | Queue WizInfo broadcast             |
| POST   | `/api/command`  | `{ "command": "text" }`      | Queue immortal command              |
| POST   | `/api/backup`   | —                              | Queue `do_backup`                   |
| POST   | `/api/shutdown` | —                              | Queue shutdown request              |

### Configuration knobs
- `WEB_ADMIN_PORT` (default `9001`): Port exposed inside the container.
- `WEB_ADMIN_HOST` (default `127.0.0.1`): Bind address for the FastAPI server (set to `0.0.0.0` only if you need remote access).
- `WEB_ADMIN_ENABLED` (default `1`): Set to `0` to skip starting the service.
- `WEB_ADMIN_QUEUE` (default `/app/area/webadmin.queue`): Where requests are queued for the game loop.
- `WEB_ADMIN_LOG_FILE` (default `/app/log/toc.log`): Log file used by `/api/logs`.

### How it works under the hood
The web service appends newline-delimited actions to `/app/area/webadmin.queue`. On each game tick, the server reads and clears the queue, then executes requests in order—broadcasting WizInfo, running immortal commands, kicking off backups, or initiating shutdown. This means queued actions are only processed while the game is running. Restarting the container clears the queue file but preserves the log file if you mounted it from the host.

> **Security note:** Expose the admin port only to trusted networks or behind a reverse proxy with authentication. The default deployment does not enforce login on the web dashboard.

## Publishing your changes to GitHub
If you don't see updates on GitHub after working locally, verify that your commits are pushed to a remote. From the repository root:

1. Confirm the remote configuration (adds one if missing):
   ```
   git remote -v
   # If nothing is listed, add your GitHub URL (example shown):
   git remote add origin https://github.com/<your-username>/tocGPT.git
   ```
2. Make sure you're on the branch you want to publish (this repository's active branch is `work`):
   ```
   git branch
   git checkout work
   ```
3. Push the branch to GitHub:
   ```
   git push origin work
   ```
4. Open a pull request on GitHub from the pushed branch. Once the PR is merged into your default branch (often `main` or `master`), the changes will appear on GitHub.

If `git push` reports authentication issues, sign in with a GitHub token or SSH key and rerun the push command.

## Applying patches with `git apply`
To apply a patch file or an inline diff, run the command from the repository root so Git can find the correct paths. For example, using three-way merge to reduce conflicts:

```
cd /path/to/tocGPT
git apply --3way /path/to/change.diff
```

If you're pasting an inline diff (like one provided in chat), wrap it with a here-doc while in the repo root:

```
cd /path/to/tocGPT
git apply --3way <<'EOF'
<paste the diff here>
EOF
```

If the patch applies cleanly, rerun `git status` to review the changes before committing.
