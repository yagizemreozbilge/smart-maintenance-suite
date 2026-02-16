@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"

echo ============================================================
echo   FRONTEND REACT TEST COVERAGE
echo ============================================================
echo.

REM ============================================================
REM PATHS
REM ============================================================

set "FRONTEND_DIR=src\frontend"
set "COV_DIR=%FRONTEND_DIR%\coverage"
set "REPORT_DIR=docs\coverage_frontend_report"
set "HISTORY_DIR=docs\coverage_history_frontend"

REM ============================================================
REM CLEAN
REM ============================================================

echo [1/5] Cleaning old coverage...

if exist "%COV_DIR%" rd /S /Q "%COV_DIR%"
if exist "%REPORT_DIR%" rd /S /Q "%REPORT_DIR%"

if not exist "%REPORT_DIR%" mkdir "%REPORT_DIR%"
if not exist "%HISTORY_DIR%" mkdir "%HISTORY_DIR%"

echo [OK] Clean complete
echo.

REM ============================================================
REM CHECK TOOLS
REM ============================================================

echo [2/5] Checking required tools...

where npm >nul 2>&1 || (echo [ERROR] npm not found & pause & exit /b 1)
where reportgenerator >nul 2>&1 || (echo [ERROR] ReportGenerator not found & pause & exit /b 1)

echo [OK] All tools found
echo.

REM ============================================================
REM INSTALL DEPENDENCIES
REM ============================================================

echo [3/5] Installing frontend dependencies...
cd "%FRONTEND_DIR%"

call npm install
if errorlevel 1 (
    echo [ERROR] npm install failed
    pause
    exit /b 1
)

echo [OK] Dependencies installed
echo.

REM ============================================================
REM RUN TESTS WITH COVERAGE
REM ============================================================

echo [4/5] Running Vitest with coverage...

call npm run test:coverage
if errorlevel 1 (
    echo [ERROR] Tests failed
    pause
    exit /b 1
)

echo [OK] Tests completed
echo.

cd ..

REM ============================================================
REM GENERATE REPORT
REM ============================================================

echo [5/5] Generating HTML report with ReportGenerator...

reportgenerator ^
    -reports:"%COV_DIR%\lcov.info" ^
    -targetdir:"%REPORT_DIR%" ^
    -historydir:"%HISTORY_DIR%" ^
    -reporttypes:Html ^
    -title:"Frontend React Tests - Coverage Report" ^
    -verbosity:Warning

if errorlevel 1 (
    echo [ERROR] Report generation failed!
    pause
    exit /b 1
)

echo.
echo ============================================================
echo   FRONTEND COVERAGE REPORT GENERATED SUCCESSFULLY
echo ============================================================
echo.
echo %cd%\%REPORT_DIR%\index.html
echo.

start "" "%cd%\%REPORT_DIR%\index.html"

pause