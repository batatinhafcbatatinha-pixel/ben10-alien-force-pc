@echo off
setlocal
cd /d "%~dp0"
python scripts\prepare_config.py
if errorlevel 1 exit /b 1
python scripts\guarded_generate.py
if errorlevel 1 exit /b 1
python scripts\make_chunks.py
exit /b %errorlevel%
