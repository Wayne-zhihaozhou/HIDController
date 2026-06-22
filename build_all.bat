@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo   HIDController Build Script
echo ========================================
echo.

:: Check for Python
python --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Python not found. Please install Python 3.8+ and add to PATH.
    exit /b 1
)
python --version

:: Check for pip
pip --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] pip not found.
    exit /b 1
)

:: Check for CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake not found. Please install CMake and add to PATH.
    exit /b 1
)
cmake --version | findstr /C:"version"

:: Check for MSBuild (Visual Studio)
set MSBUILD=
for %%v in (
    "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
) do set "VSWHERE=%%v"

if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%m in (`"%VSWHERE%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe 2^>nul`) do (
        if exist "%%m" set "MSBUILD=%%m"
    )
)

if not defined MSBUILD (
    echo [ERROR] Visual Studio (Desktop C++ workload) not found.
    echo        Please install "Desktop development with C++" from VS Installer.
    exit /b 1
)
echo MSBuild found: !MSBUILD!
echo.

:: Step 1: Build DLL
echo ========================================
echo   Step 1: Building HIDController.dll
echo ========================================
echo.

pushd library
if exist "build" rmdir /s /q build
call "!MSBUILD!" HIDController.sln -p:Configuration=Release -p:Platform=x64 -m -restore:true -verbosity:minimal
popd

if errorlevel 1 (
    echo [ERROR] DLL build failed.
    exit /b 1
)

echo [OK] DLL build successful.
echo.

:: Step 2: Build Python extensions
echo ========================================
echo   Step 2: Building Python extensions
echo ========================================
echo.

pushd python
powershell -ExecutionPolicy Bypass -File build.ps1
popd

if errorlevel 1 (
    echo [ERROR] Python extension build failed.
    exit /b 1
)

echo [OK] Python extensions built successfully.
echo.

:: Step 3: Copy DLL to package directory
echo ========================================
echo   Step 3: Installing artifacts
echo ========================================
echo.

copy /y library\build\Release\HIDController.dll python\HIDController\ >nul
echo   Copied HIDController.dll to python/HIDController/

echo.
echo ========================================
echo   Build Complete!
echo ========================================
echo.
echo   DLL:       library\build\Release\HIDController.dll
echo   Python:    python\HIDController\
echo.
echo   Install:   pip install -e python\
echo   Or:        cd python && pip install .
echo.
