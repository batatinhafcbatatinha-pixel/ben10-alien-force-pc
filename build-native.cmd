@echo off
setlocal
set "VSLANG=1033"
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
"%~dp0..\recomp-python\Scripts\python.exe" "%~dp0scripts\build_native.py" %*
exit /b %errorlevel%
