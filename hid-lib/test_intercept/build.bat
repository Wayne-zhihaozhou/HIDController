@echo off
setlocal

:: Build hid_controller.lib first (dependency)
echo === Building hid_controller ===
call "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" ^
    "%~dp0..\hid_controller.vcxproj" ^
    /p:Configuration=Release /p:Platform=x64 /m /nologo /v:minimal

if %errorlevel% neq 0 (
    echo FAILED: hid_controller build
    pause
    exit /b 1
)

:: Build test_intercept
echo.
echo === Building test_intercept ===
call "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" ^
    "%~dp0test_intercept.vcxproj" ^
    /p:Configuration=Release /p:Platform=x64 /m /nologo /v:minimal

if %errorlevel% neq 0 (
    echo FAILED: test_intercept build
    pause
    exit /b 1
)

echo.
echo === Build OK ===
echo Run: build\Release\test_intercept.exe
pause
