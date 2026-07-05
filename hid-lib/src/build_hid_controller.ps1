# build_hid_controller.ps1
# Build hid_controller.vcxproj (Release|x64) using MSBuild from VS2022

$ErrorActionPreference = 'Continue'

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$vcxproj   = Join-Path $ScriptDir "..\hid_controller.vcxproj"

if (-not (Test-Path $vcxproj)) {
    Write-Error "vcxproj not found: $vcxproj"
    exit 1
}

Write-Host "=== Build hid_controller (Release|x64) ===" -ForegroundColor Cyan

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

$buildResult = $LASTEXITCODE
if ($buildResult -ne 0) {
    Write-Error "Build failed with exit code $buildResult"
    exit $buildResult
}

Write-Host ""
Write-Host "=== Build succeeded ===" -ForegroundColor Green

$buildDir = Join-Path $ScriptDir "..\build\Release"

# Copy header to build output
Copy-Item "$ScriptDir\..\include\hid_controller.h" $buildDir -Force
Write-Host "Header : $buildDir\hid_controller.h" -ForegroundColor Yellow

# Copy docs to build output
Copy-Item "$ScriptDir\..\..\docs\api-reference.md" $buildDir -Force
Write-Host "Docs   : $buildDir\api-reference.md" -ForegroundColor Yellow

# Remove intermediate files
foreach ($name in @("hid_controller.exp", "hid_controller.pdb")) {
    $target = Join-Path $buildDir $name
    if (Test-Path $target) {
        Remove-Item $target -Force
        Write-Host "Removed: $target" -ForegroundColor DarkGray
    }
}

Write-Host "Output : $buildDir\hid_controller.dll" -ForegroundColor Yellow
