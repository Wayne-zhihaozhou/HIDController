# build.ps1 - Build HIDController Python extensions
param(
    [string]$Config = "Release",
    [switch]$Clean,
    [switch]$Help
)

$ErrorActionPreference = "Stop"
$PSScriptRoot = Split-Path $MyInvocation.MyCommand.Path -Parent
$BuildDir = Join-Path $PSScriptRoot "build"
$CMakeDir = Join-Path $BuildDir "cmake"

if ($Help) {
    Write-Host "Usage: .\build.ps1 [-Config Debug|Release] [-Clean] [-Help]" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -Config    Build configuration (default: Release)"
    Write-Host "  -Clean     Remove build directory before building"
    Write-Host "  -Help      Show this help message"
    exit 0
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  HIDController Python Extension Build" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check Python
$pythonExe = "python"
try {
    $pyVer = & $pythonExe --version 2>&1
    Write-Host "Python: $pyVer" -ForegroundColor Green
} catch {
    Write-Host "[ERROR] Python not found. Please install Python 3.8+." -ForegroundColor Red
    exit 1
}

# Check CMake
try {
    $cmakeVer = cmake --version 2>&1 | Select-String "version" | Select-Object -First 1
    Write-Host "CMake: $cmakeVer" -ForegroundColor Green
} catch {
    Write-Host "[ERROR] CMake not found. Please install CMake and add to PATH." -ForegroundColor Red
    exit 1
}

# Find MSBuild
$msbuild = $null
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
    $msbuildPath = & $vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe 2>$null
    if ($msbuildPath) { $msbuild = $msbuildPath.Trim() }
}
if (-not $msbuild) {
    Write-Host "[ERROR] Visual Studio (Desktop C++ workload) not found." -ForegroundColor Red
    exit 1
}
Write-Host "MSBuild: $msbuild" -ForegroundColor Green
Write-Host ""

# Clean if requested
if ($Clean -and (Test-Path $CMakeDir)) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item $CMakeDir -Recurse -Force
}

# Create build dir
if (!(Test-Path $CMakeDir)) {
    New-Item -ItemType Directory -Path $CMakeDir | Out-Null
}

Write-Host "Configuring CMake (Build dir: $CMakeDir)..." -ForegroundColor Cyan
Push-Location $CMakeDir
try {
    cmake "$PSScriptRoot" `
        -G "Visual Studio 17 2022" `
        -A x64 `
        -DPython_EXECUTABLE="$pythonExe" `
        -DCMAKE_BUILD_TYPE=$Config 2>&1 | Out-Host
} catch {
    Write-Host "[ERROR] CMake configuration failed." -ForegroundColor Red
    Pop-Location
    exit 1
}

Write-Host ""
Write-Host "Building..." -ForegroundColor Cyan
cmake --build . --config $Config --parallel -- /verbosity:minimal 2>&1 | Out-Host
$exitCode = $LASTEXITCODE

Pop-Location

if ($exitCode -ne 0) {
    Write-Host ""
    Write-Host "[ERROR] Build failed." -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "[OK] Build successful!" -ForegroundColor Green
Write-Host ""

# Show built files
$pydFiles = Get-ChildItem $CMakeDir -Recurse -Filter "*.pyd" 2>$null
if ($pydFiles) {
    Write-Host "Built extensions:" -ForegroundColor Green
    foreach ($f in $pydFiles) {
        Write-Host "  $($f.FullName)" -ForegroundColor White
    }
}

Write-Host ""
Write-Host "To use:" -ForegroundColor Cyan
Write-Host "  pip install -e $PSScriptRoot" -ForegroundColor White
Write-Host ""
