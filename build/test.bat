@echo off

echo;
echo =========================================
echo Test YaizuSample
echo =========================================

if defined GITHUBACTIONS (
  echo For GitHub Actions
  set MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\MSBuild\15.0\Bin\msbuild.exe"
  set DEVENV="C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\Common7\IDE\devenv.exe"
  set SEVENZIP="7z.exe"
  set LCOUNTER=""
  goto definitionend
)

set LOCALMACHINE="true"

set MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\msbuild.exe"
set DEVENV="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv.exe"
set SEVENZIP="C:\Program Files\7-Zip\7z.exe"
set LCOUNTER="C:\Program Files (x86)\lcounter\lcounter.exe"

echo;
echo This batch file requires softwares shown below.
echo 1. Microsoft Visual Studio 2017
echo 2. 7-Zip 9.20
echo 3. Line Counter

if not exist %MSBUILD% (
  exit
) else if not exist %DEVENV% (
  exit
) else if not exist %SEVENZIP% (
  exit
) else if not exist %LCOUNTER% (
  exit
)

:definitionend


rem ########## Initializing ##########
echo;
echo Initializing...
if exist server rmdir /S /Q server


rem ########## Building ##########
echo;
echo Building sample.sln...
rem Considering execution privilege, .exe is built as Debug. (Debug=AsInvoker, Release=AsAdmin)
%MSBUILD% "..\src\sample\sample.sln" /t:clean;build /p:Configuration=Debug
IF %ERRORLEVEL% NEQ 0 goto ERRORRAISED
echo Building sampletest.sln...
%MSBUILD% "..\test\sampletest\sampletest.sln" /t:clean;build /p:Configuration=Release
IF %ERRORLEVEL% NEQ 0 goto ERRORRAISED


rem ########## Checking file existence ##########
echo;
echo Checking "sample.exe" existence...
if not exist "..\src\sample\Debug\sample.exe" goto ERRORRAISED
echo Checking "sampletest.exe" existence...
if not exist "..\test\sampletest\Release\sampletest.exe" goto ERRORRAISED


rem ########## Deployment of files and folders ##########
echo;
echo Deployment of files and folders...
mkdir server
copy "..\src\sample\Debug\sample.exe" server
copy "..\src\sample\sample.dat" server
copy "..\src\sample\sample.conf" server
echo servicehost=localhost>> server\sample.conf
echo serviceport=10009>> server\sample.conf
copy "..\test\sampletest\Release\sampletest.exe" server


rem ########## Testing ##########
echo;
echo Test starts
start server\sample.exe
server\sampletest.exe
echo Test ends


rem ########## build complete ##########
echo;
echo All testing processes of YaizuSample have been successfully finished.
if defined LOCALMACHINE (
  pause
)
exit /B %ERRORLEVEL%


rem ########## Error ##########
:ERRORRAISED
echo;
echo Build error.
if defined LOCALMACHINE (
  pause
)
exit /B 1

