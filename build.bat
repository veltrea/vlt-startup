@echo off
setlocal
cd /d "%~dp0"

if not exist bin mkdir bin

echo Compiling vlt-startup (Folder method)...
cl.exe /EHsc /O2 /W3 src/vlt-startup.cpp /Febin/vlt-startup.exe /Isrc Ole32.lib Shell32.lib Advapi32.lib User32.lib

echo Compiling vlt-autorun (Registry method)...
cl.exe /EHsc /O2 /W3 src/vlt-autorun.cpp /Febin/vlt-autorun.exe /Isrc Advapi32.lib Shlwapi.lib

echo Compiling vlt-task (Task method)...
cl.exe /EHsc /O2 /W3 src/vlt-task.cpp /Febin/vlt-task.exe /Isrc /I. Advapi32.lib Shlwapi.lib User32.lib Ole32.lib OleAut32.lib

if %ERRORLEVEL% equ 0 (
    echo All tools build successful in bin/ directory.
) else (
    echo Build failed.
    exit /b %ERRORLEVEL%
)
