@echo off
rem Fast incremental edit/test loop for Ben 10 Alien Force.
setlocal
python "%~dp0scripts\check_build_space.py"
if errorlevel 1 exit /b 1
if not exist "%~dp0build\game\build.ninja" (
  echo Configure once with build-game.cmd full before using this command.
  exit /b 1
)
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
set "JOBS=%~2"
if not defined JOBS set "JOBS=2"
if /i "%~1"=="runtime" goto exe
cmake --build "%~dp0build\game" --target ps2_game_code --parallel %JOBS%
if errorlevel 1 exit /b 1
:exe
cmake --build "%~dp0build\game" --target ps2EntryRunner --parallel %JOBS%
exit /b %errorlevel%
