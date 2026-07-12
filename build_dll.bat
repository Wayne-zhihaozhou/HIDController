@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
MSBuild.exe "C:\Users\Admin\source\repos\HIDController\hid-lib\hid_controller.vcxproj" /p:Configuration=Release /p:Platform=x64 /v:m
