@echo off
if exist vlt-debug.log del vlt-debug.log
call run_build.bat
.\bin\vlt-task.exe -a "c:\windows\system32\notepad.exe" --name "CTimeLogTest"
type vlt-debug.log
