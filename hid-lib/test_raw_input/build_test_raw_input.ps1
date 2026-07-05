# build_test_raw_input.ps1
# Build test_raw_input.vcxproj (Release|x64) using MSBuild from VS2022

$ErrorActionPreference = 'Stop'

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$vcxproj   = Join-Path $ScriptDir "test_raw_input.vcxproj"

if (-not (Test-Path $vcxproj)) {
    Write-Error "vcxproj not found: $vcxproj"
    exit 1
}

Write-Host "=== Build test_raw_input (Release|x64) ===" -ForegroundColor Cyan

# Locate MSBuild — supports Community / Professional / Enterprise
$vsInstall = "${env:ProgramFiles}\Microsoft Visual Studio\2022"
$foundMsbuild = $null
foreach ($edition in @('Community', 'Professional', 'Enterprise')) {
    $candidate = $vsInstall + "\" + $edition + "\MSBuild\Current\Bin\MSBuild.exe"
    if (Test-Path $candidate) { $foundMsbuild = $candidate; break }
}

if (-not $foundMsbuild) {
    Write-Error "MSBuild.exe not found under $vsInstall. Is VS2022 installed?"
    exit 1
}

$msbuild = $foundMsbuild

Write-Host "msbuild : $msbuild"
Write-Host "project : $vcxproj"
Write-Host ""

& $msbuild $vcxproj `
    /t:Build `
    /p:Configuration=Release `
    /p:Platform=x64 `
    /v:minimal

if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed with exit code $LASTEXITCODE"
    exit $LASTEXITCODE
}

Write-Host ""
Write-Host "=== Build succeeded ===" -ForegroundColor Green

$output = Join-Path $ScriptDir "build\Release\test_raw_input.exe"
if (Test-Path $output) {
    Write-Host "Output : $output" -ForegroundColor Yellow
}
