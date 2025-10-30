# Windows Launcher

The Windows launcher provides a portable executable that can start, monitor, and
restart the MUD. It includes both a GUI and console interface and displays the
information and wizinfo logs in real time.

## Building

### One-click build on Windows 11

Run the bundled PowerShell script from an elevated terminal to install the
required toolchain (if missing) and produce a portable launcher executable:

```
powershell -ExecutionPolicy Bypass -File launcher\build_launcher.ps1
```

The script performs the following tasks:

1. Installs the **Visual Studio 2022 Build Tools** with the MSVC C++ workload
   through `winget` when they are not already present.
2. Locates the `vcvars*.bat` environment script that ships with MSVC.
3. Invokes `cl.exe` with the correct libraries and writes the resulting
   `win_launcher.exe` to `launcher\bin\`.

To generate a debug build instead, run the script with `-Configuration Debug`.

### Manual build

The launcher is a single C++ source file that only depends on the Win32 API. It
can also be compiled manually with the Microsoft Visual C++ compiler that ships
with Visual Studio or with the Build Tools after the environment is set up:

```
"%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && \
cl /std:c++17 /W4 /EHsc launcher\win_launcher.cpp user32.lib gdi32.lib shell32.lib comdlg32.lib ole32.lib
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
