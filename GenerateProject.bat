@echo off
echo Generating Visual Studio 2022 solution...

REM Check if premake5.exe exists
if not exist "vendor\bin\premake\premake5.exe" (
    echo ERROR: premake5.exe not found at vendor\bin\premake\premake5.exe
    echo Please make sure Premake5 is installed in the correct location.
    pause
    exit /b 1
)

REM Check if premake5.lua exists
if not exist "premake5.lua" (
    echo ERROR: premake5.lua not found in current directory
    echo Please run this script from the project root directory.
    pause
    exit /b 1
)

REM Clean previous build files (optional but recommended)
echo Cleaning previous build files...
if exist "*.sln" del /q "*.sln"
if exist "*.vcxproj" del /q "*.vcxproj*"
if exist "bin" rmdir /s /q "bin"
if exist "bin-int" rmdir /s /q "bin-int"

REM Generate the project files
echo Running Premake5...
call vendor\bin\premake\premake5.exe vs2022

REM Check if generation was successful
if %ERRORLEVEL% neq 0 (
    echo ERROR: Premake5 failed to generate project files
    echo Check the premake5.lua file for syntax errors
    pause
    exit /b 1
)

echo.
echo Project files generated successfully!
echo You can now open the .sln file in Visual Studio 2022
echo.

PAUSE