@echo off
rem Headless Ghidra import/analysis of SLUS_218.15 (EE extension) + PS2Recomp function export.
setlocal
set "GHIDRA=C:\Users\dudua\Downloads\ghidra_12.1.4_PUBLIC_20260921\ghidra_12.1.4_PUBLIC"
set "SCRIPTS=%~dp0..\third_party\PS2Recomp\ps2xRecomp\tools\ghidra"
if not exist "%GHIDRA%\support\analyzeHeadless.bat" (
  echo Ghidra not found at "%GHIDRA%". Please verify the installation path.
  exit /b 1
)
call "%GHIDRA%\support\analyzeHeadless.bat" "%~dp0ghidra" Ben10AF -import "%~dp0analysis\disc\SLUS_218.15" -overwrite ^
  -processor "r5900:LE:32:default" -analysisTimeoutPerFile 600 ^
  -scriptPath "%SCRIPTS%" -postScript ExportPS2Functions.java "%~dp0config\ben10-af.toml" "%~dp0config\ghidra-functions.csv"
if errorlevel 1 exit /b 1
call "%GHIDRA%\support\analyzeHeadless.bat" "%~dp0ghidra" Ben10AF -process SLUS_218.15 -noanalysis -readOnly ^
  -scriptPath "%~dp0scripts" -postScript ExportBen10FunctionsOnly.java "%~dp0config\ghidra-functions-only.csv"
exit /b %errorlevel%
