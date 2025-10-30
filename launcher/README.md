# Windows Launcher

The Windows launcher provides a portable executable that can start, monitor, and
restart the MUD. It includes both a GUI and console interface and displays the
information and wizinfo logs in real time.

## Building

The launcher is a single C++ source file that only depends on the Win32 API. It
can be compiled with the Microsoft Visual C++ compiler that ships with Visual
Studio or with the Build Tools.

```
cl /std:c++17 /W4 /EHsc launcher\win_launcher.cpp user32.lib gdi32.lib shell32.lib
```

The resulting `win_launcher.exe` is portable. Place it in the same directory as
`toc.exe` after compilation.

## Configuration

`launcher_config.ini` (optional) controls the executable and log file locations.
Paths are relative to the launcher executable by default. Copy
`launcher_config.sample` to `launcher_config.ini` to customise the defaults.

```
mud_executable=.\toc.exe
working_directory=.
info_log=.\log\info.log
wiz_log=.\log\wizinfo.log
auto_restart=1
```

Set `auto_restart` to `0` to disable automatic restarts.

## Usage

* Run `win_launcher.exe` normally to open the GUI. Use the **Start/Stop** button
  to control the MUD and view live log updates.
* Run `win_launcher.exe --console` to use the text-based interface.
* In both modes, the launcher keeps the MUD running and will restart it after an
  unexpected exit when auto-restart is enabled.
