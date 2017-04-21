@ECHO OFF
:: Windows batch commands - https://ss64.com/nt/

IF "%1"=="" goto show_help

::Visual C++ 2005
if "%1"=="vc8" set VS_DIR=%VS80COMNTOOLS:~,-15%\VC  

:: Visual C++ 2008
if "%1"=="vc9" set VS_DIR=%VS90COMNTOOLS:~,-15%\VC

:: Visual C++ 2010
if "%1"=="vc10" set VS_DIR=%VS100COMNTOOLS:~,-15%\VC

:: Visual C++ 2012
if "%1"=="vc11" set VS_DIR=%VS110COMNTOOLS:~,-15%\VC

:: Visual C++ 2013
if "%1"=="vc12" set VS_DIR=%VS120COMNTOOLS:~,-15%\VC

:: Visual C++ 2015
if "%1"=="vc14" set VS_DIR=%VS140COMNTOOLS:~,-15%\VC

:: SET FOR BUILD X86 VERSION BY DEFAULT IF NOT SETUP
if "%BUILD_MAKE_VERSION%"=="" set BUILD_MAKE_VERSION=x86

set PATH=%PATH%;%VS_DIR%
call "%VS_DIR%/vcvarsall.bat" %BUILD_MAKE_VERSION%

if "%3"=="" goto build_all_projects
if "%4"=="" goto build_one_project

goto show_help

:build_one_project
echo "Build %2 => %3 in debug mode for %1"
DevEnv %2 /build debug   /project %3
if errorlevel 1 (
   echo Failure build %2=>%3 in debug mode. Return code is %errorlevel%. Maybe ok.
)
goto :eof

:build_all_projects
echo "Build all projects from %2 in debug mode for %1"
DevEnv %2 /build debug
if errorlevel 1 (
   echo Failure build %2 in debug mode. Return code is %errorlevel%. Maybe ok.
)
goto :eof

:show_help

echo "COMAND LINE FOR ONE PROJECT BUILDING:                          vc{X}, SolutionPath, ProjectName"
echo "COMAND LINE FOR ALL PROJECTS BUILDING:                         vc{X}, SolutionPath"
goto :eof

:eof
