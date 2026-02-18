@echo off
setlocal EnableExtensions EnableDelayedExpansion

cd /d "%~dp0"

echo ============================================================
echo   FRONTEND REACT TEST COVERAGE
echo ============================================================
echo.
echo Current Directory: %cd%
echo.

:: Tool checks
echo [DEBUG] Checking npm...
call npm --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] npm not found! Is Node.js installed?
    pause
    exit /b 1
)

echo [DEBUG] Checking ReportGenerator...
where reportgenerator >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] ReportGenerator not found! 
    echo Please install with: dotnet tool install -g dotnet-reportgenerator-globaltool
    pause
    exit /b 1
)

set "ROOT_DIR=%~dp0"
set "FRONTEND_DIR=%ROOT_DIR%src\frontend"
set "COV_DIR=%FRONTEND_DIR%\coverage"
set "REPORT_DIR=%ROOT_DIR%docs\coverage_frontend_report"

echo [1/3] Cleaning old reports...
if exist "%COV_DIR%" rd /S /Q "%COV_DIR%"
if not exist "%REPORT_DIR%" mkdir "%REPORT_DIR%"

echo [2/3] Running Tests and Coverage...
cd /d "%FRONTEND_DIR%"
call npm run test:coverage
set "EXIT_CODE=%errorlevel%"

echo.
echo [DEBUG] Vitest Exit Code: %EXIT_CODE%
echo.

if not exist "%COV_DIR%\lcov.info" (
    echo [ERROR] lcov.info was not generated! 
    echo Path: %COV_DIR%
    dir "%COV_DIR%" 2>nul
    echo.
    echo Please check Vitest configuration.
    pause
    exit /b 1
)

echo [3/3] Generating Report...
cd /d "%ROOT_DIR%"
reportgenerator ^
    -reports:"%COV_DIR%\lcov.info" ^
    -targetdir:"%REPORT_DIR%" ^
    -reporttypes:Html ^
    -sourcedirs:"src\frontend" ^
    -title:"Frontend Coverage Report (Windows)"

if %errorlevel% neq 0 (
    echo [ERROR] ReportGenerator failed.
    pause
    exit /b 1
)

echo.
echo ============================================================
echo   PROCESS COMPLETED SUCCESSFULLY
echo ============================================================
echo Report: %REPORT_DIR%\index.html
echo.

start "" "%REPORT_DIR%\index.html"
pause