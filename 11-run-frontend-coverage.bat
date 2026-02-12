@echo off
echo ==========================================
echo Frontend Test Coverage Baslatiliyor...
echo ==========================================

REM Proje kokunden frontend klasorune git
cd /d C:\Users\yagiz\Desktop\Project\smart-maintenance-suite\src\frontend

echo.
echo Coverage calistiriliyor...
call npm run test:coverage

echo.
echo Coverage raporu kontrol ediliyor...

REM Coverage tests/frontend altinda olusuyor
if exist ..\tests\frontend\coverage\lcov-report\index.html (
    echo Coverage raporu aciliyor...
    start ..\tests\frontend\coverage\lcov-report\index.html
) else (
    echo Coverage raporu bulunamadi!
)

pause