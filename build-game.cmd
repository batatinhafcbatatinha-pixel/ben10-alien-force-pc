@echo off
rem Compatibility wrapper: native build, no game execution.
setlocal
if /i "%~1"=="full" (
  call "%~dp0build-native.cmd"
) else (
  call "%~dp0build-native.cmd" %*
)
exit /b %errorlevel%
