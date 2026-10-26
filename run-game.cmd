@echo off
setlocal
set "GAME_ISO=D:\ISO\Ben 10 - Alien Force (USA) (En,Fr,De,Es,It)\Ben 10 - Alien Force (USA) (En,Fr,De,Es,It).iso"
if not "%~1"=="" set "GAME_ISO=%~f1"
if not exist "%GAME_ISO%" (
  echo ISO not found: "%GAME_ISO%"
  exit /b 1
)
set "BIN_DIR=%~dp0build\native\ps2recomp\ps2xRuntime"
if exist "%~dp0build-location.txt" set /p BIN_DIR=<"%~dp0build-location.txt"
if not exist "%BIN_DIR%\ps2EntryRunner.exe" (
  echo Build the runner first with build-native.cmd.
  exit /b 1
)
if not "%~2"=="" set "PS2X_WINDOW_MODE=%~2"
cd /d "%~dp0"
"%BIN_DIR%\ps2EntryRunner.exe" "%~dp0game_files\SLUS_218.15" "%GAME_ISO%"
exit /b %errorlevel%
