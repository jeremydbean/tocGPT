Please see the WIKI for building documents and Raspberry Pi install info.
CURRENT REPORTED BUGS:
- Game crashes at login if pfile is set with a (NULL) password.
        WORKAROUND: RESET PFILE PASSWORDS TO:  Kyz2D/BNiZB8Q~  (which translates to toc123)
<strike>- Game currently allows players to log in multiple times.  (Many copies of the character can be loaded.)   This is modified via ChatGPT to be compileable on Ubuntu 24 LTS.</strike>
sudo chmod -R 777 tocGPT
sudo chmod a+rwx -R tocGPT

## Docker usage
You can build and run the MUD inside a portable Docker container that exposes the game port on all interfaces. The steps below cover a brand-new install of macOS, Windows, and Ubuntu, followed by common Docker commands for building and running the container anywhere.

### macOS (new install)
1. Install Docker Desktop from <https://www.docker.com/products/docker-desktop> and start it once so it can finish setup.
2. Clone this repository:
   ```
   git clone https://github.com/jgmeiner/tocGPT.git
   cd tocGPT
   ```
3. Build and run (see [Common Docker commands](#common-docker-commands)).

### Windows (new install)
1. Install Docker Desktop for Windows from <https://www.docker.com/products/docker-desktop>. Enable WSL 2 integration if prompted, then start Docker Desktop.
2. Use PowerShell or Git Bash to clone the repository:
   ```powershell
   git clone https://github.com/jgmeiner/tocGPT.git
   cd tocGPT
   ```
3. Build and run using the commands in [Common Docker commands](#common-docker-commands). When mapping volumes, replace `$(pwd)` with `${PWD}` in PowerShell or `$(pwd)` in Git Bash.

### Ubuntu (new install)
1. Install Docker Engine:
   ```
   sudo apt update
   sudo apt install -y docker.io
   sudo systemctl enable --now docker
   sudo usermod -aG docker "$USER"
   ```
   Log out and back in (or `newgrp docker`) so your user can run Docker without sudo.
2. Clone the repository:
   ```
   git clone https://github.com/jgmeiner/tocGPT.git
   cd tocGPT
   ```
3. Use the [Common Docker commands](#common-docker-commands) to build and run the container.

### Common Docker commands
1. Build the image from the repository root:
   ```
   docker build -t toc-mud .
   ```
2. Start the server (default port 9000) and forward it to your host for local connections:
   ```
   docker run --rm -it -p 9000:9000 toc-mud
   ```
3. To change the port, set the `PORT` (or `MUD_PORT`) environment variable when running:
   ```
   docker run --rm -it -e PORT=4000 -p 4000:4000 toc-mud
   ```
4. For persistent characters and logs across container runs, mount the data directories from your host:
   ```
   docker run --rm -it \
     -p 9000:9000 \
     -v $(pwd)/player:/app/player \
     -v $(pwd)/backups:/app/backups \
     -v $(pwd)/log:/app/log \
     toc-mud
   ```

The container automatically creates the writable data directories (`player`, `backups`, `log`, `gods`, `heroes`, `corpse`) on startup so that fresh volumes are ready for the game server. If you mount host directories, ensure they are writable by the container user.

The container entrypoint accepts optional arguments if you need to pass flags directly to the `merc` binary (for example, `newlock` to block new players). If no arguments are provided, it automatically starts the server on the configured port.

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
