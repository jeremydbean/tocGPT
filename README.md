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
and wizinfo logs is available under `launcher/`. Build it with the standard
Microsoft toolchain and place the resulting executable alongside `toc.exe`.
See `launcher/README.md` for full instructions.
