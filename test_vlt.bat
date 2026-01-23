@echo off
setlocal
cd /d "%~dp0"

echo === vlt-schtask Test Suite ===

:: 管理者権限チェック
openfiles >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [ERROR] This script MUST be run as Administrator.
    echo Please right-click and select "Run as administrator".
    pause
    exit /b 1
)

echo [OK] Running with Administrator privileges.
echo.

set EXE=bin\vlt-schtask.exe
set TEST_TARGET=c:\windows\system32\notepad.exe
set SPACE_TARGET="%~dp0bin\vlt-schtask.exe"

echo 1. Simple Add test (notepad.exe)...
%EXE% -a %TEST_TARGET% --name "VltTestSimple"
if %ERRORLEVEL% equ 0 (echo [OK]) else (echo [FAILED])
echo.

echo 2. Path with spaces and Japanese test (this tool itself)...
%EXE% -a %SPACE_TARGET% --name "VltTestSpace"
if %ERRORLEVEL% equ 0 (echo [OK]) else (echo [FAILED])
echo.

echo 3. Add with arguments test...
%EXE% -a %TEST_TARGET% --name "VltTestArgs" --args "test_argument_message"
if %ERRORLEVEL% equ 0 (echo [OK]) else (echo [FAILED])
echo.

echo 4. Remove tests...
%EXE% -r "VltTestSimple"
%EXE% -r "VltTestSpace"
%EXE% -r "VltTestArgs"
echo.

echo === Test finished ===
pause
