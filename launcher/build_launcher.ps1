[CmdletBinding()]
param(
    [string]$Configuration = "Release",
    [string]$OutputDirectory = "bin"
)

$ErrorActionPreference = 'Stop'

function Assert-CommandExists {
    param(
        [Parameter(Mandatory = $true)][string]$Command
    )

    if (-not (Get-Command $Command -ErrorAction SilentlyContinue)) {
        throw "Required command '$Command' was not found on PATH."
    }
}

function Ensure-BuildTools {
    $packageId = "Microsoft.VisualStudio.2022.BuildTools"
    $cxxComponent = "Microsoft.VisualStudio.Component.VC.Tools.x86.x64"

    Write-Host "Checking for Visual Studio Build Tools..."

    $vswherePath = Join-Path -Path ${env:ProgramFiles(x86)} -ChildPath "Microsoft Visual Studio\\Installer\\vswhere.exe"
    if (-not (Test-Path $vswherePath)) {
        Write-Host "vswhere not found; attempting to install Visual Studio Build Tools via winget." -ForegroundColor Yellow
    }

    $installed = $false
    if (Test-Path $vswherePath) {
        $installedPath = & $vswherePath -latest -products * -requires $cxxComponent -property installationPath
        if ($LASTEXITCODE -eq 0 -and $installedPath) {
            $installed = $true
        }
    }

    if (-not $installed) {
        Write-Host "Installing Visual Studio Build Tools (this may take several minutes)..." -ForegroundColor Yellow
        Assert-CommandExists -Command "winget"
        winget install --id $packageId -e --accept-package-agreements --accept-source-agreements --override "--wait --norestart --add $cxxComponent" | Out-Null
    }

    if (-not (Test-Path $vswherePath)) {
        throw "vswhere.exe was not found after installation. Please ensure Visual Studio Build Tools are installed."
    }

    $installationPath = & $vswherePath -latest -products * -requires $cxxComponent -property installationPath
    if ($LASTEXITCODE -ne 0 -or -not $installationPath) {
        throw "Unable to locate Visual Studio Build Tools with the required C++ workload."
    }

    return $installationPath
}

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Definition
$source = Join-Path $scriptRoot 'win_launcher.cpp'
if (-not (Test-Path $source)) {
    throw "Unable to locate win_launcher.cpp next to the script."
}

$installationPath = Ensure-BuildTools

$vcvars64 = Join-Path $installationPath 'VC\\Auxiliary\\Build\\vcvars64.bat'
$vcvars32 = Join-Path $installationPath 'VC\\Auxiliary\\Build\\vcvars32.bat'

$toolchainBatch = if (Test-Path $vcvars64) { $vcvars64 } elseif (Test-Path $vcvars32) { $vcvars32 } else { throw "Could not find vcvars batch script in the Visual Studio installation." }

$buildDir = Join-Path $scriptRoot $OutputDirectory
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

$target = Join-Path $buildDir 'win_launcher.exe'

$defines = @()
if ($Configuration -ieq 'Release') {
    $defines += '/O2'
} else {
    $defines += '/Od'
    $defines += '/Zi'
}

$clArgs = @(
    '/nologo',
    '/std:c++17',
    '/W4',
    '/EHsc',
    '/DUNICODE',
    '/D_UNICODE'
)
$clArgs += $defines
$clArgs += @("`"$source`"", 'user32.lib', 'gdi32.lib', 'shell32.lib', 'comdlg32.lib', 'ole32.lib', '/Fe' + "`"$target`"")

$command = "`"$toolchainBatch`" && cl $($clArgs -join ' ')"

Write-Host "Building win_launcher.exe..."
cmd /c $command

if ($LASTEXITCODE -ne 0) {
    throw "Compilation failed (exit code $LASTEXITCODE)."
}

Write-Host "Launcher built successfully at $target" -ForegroundColor Green
