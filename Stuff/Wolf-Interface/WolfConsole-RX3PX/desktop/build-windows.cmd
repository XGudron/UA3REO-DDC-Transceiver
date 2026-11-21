@echo off
setlocal
cd /d "%~dp0"
where npm.cmd >nul 2>nul || (echo Install Node.js 22 LTS x64 and reopen this script. & pause & exit /b 1)
call npm.cmd ci --no-audit --no-fund || goto fail
call npm.cmd run exe || goto fail
start "" "%cd%\dist"
exit /b 0
:fail
echo Build failed. See the error above.
pause
exit /b 1
