@echo off
setlocal
set "PS2X_CD_IMAGE="

set "BIN_DIR=%~dp0build\native\ps2recomp\ps2xRuntime"
if exist "%~dp0build-location.txt" set /p BIN_DIR=<"%~dp0build-location.txt"
if not exist "%BIN_DIR%\ps2EntryRunner.exe" (
  echo Build the runner first with build-native.cmd.
  exit /b 1
)
if not "%~2"=="" set "PS2X_WINDOW_MODE=%~2"
cd /d "%~dp0"
if not exist "%~dp0game_files\.disc\layout.tsv" (
  python "%~dp0tools\port-installer\prepare_folder.py" --root "%~dp0." --config "%~dp0installer.json"
  if errorlevel 1 exit /b 1
)
"%BIN_DIR%\ps2EntryRunner.exe" "%~dp0game_files\SLUS_218.15"
exit /b %errorlevel%
