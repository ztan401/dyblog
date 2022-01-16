@echo off
REM 声明采用UTF-8编码
chcp 65001
title vue web

@echo off
reg add HKEY_CURRENT_USER\Console /v QuickEdit /t REG_DWORD /d 00000000 /f

echo hello from vue!

start %LOCALAPPDATA%/min/min.exe http://localhost:8080/

npm run serve
pause