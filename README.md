Please see the WIKI for building documents and Raspberry Pi install info.

CURRENT REPORTED BUGS:
- Game crashes at login if pfile is set with a (NULL) password.
        WORKAROUND: RESET PFILE PASSWORDS TO:  Kyz2D/BNiZB8Q~  (which translates to toc123)
<strike>- Game currently allows players to log in multiple times.  (Many copies of the character can be loaded.)   This is modif
ied via ChatGPT to be compileable on Ubuntu 24 LTS.
sudo chmod -R 777 tocGPT
sudo chmod a+rwx -R tocGPT

## Windows Launcher

A portable Windows launcher that keeps the MUD running and surfaces informational
and wizinfo logs is available under `launcher/`.

* **One-click build:** run `powershell -ExecutionPolicy Bypass -File launcher\build_launcher.ps1`
  from an elevated terminal on Windows 11. The script installs the required
  Visual Studio Build Tools (via `winget`) when missing and emits
  `launcher\bin\win_launcher.exe`.
* **Manual build:** follow the MSVC instructions in `launcher/README.md` if you
  already have the Windows C++ toolchain configured.

Place the resulting executable alongside `toc.exe` once compiled.
