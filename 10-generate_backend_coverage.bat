@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"

call venv_win\Scripts\activate.bat

REM ------------------------------------------------------------
REM Ensure MSYS2 MinGW64 tools are in PATH
REM ------------------------------------------------------------
set "PATH=C:\msys64\mingw64\bin;%PATH%"

set "ROOT=%cd%"
set "BUILD_DIR=build_backend_tests"
set "COV_DIR=coverage_backend"
set "REPORT_DIR=docs\coverage_backend_report"
set "HISTORY_DIR=docs\coverage_history"

echo ============================================================
echo   BACKEND GCC COVERAGE - FULL TEST SUITE (29 TESTS)
echo ============================================================
echo.

REM ============================================================
REM CLEAN
REM ============================================================

echo [1/5] Cleaning old build and coverage data...

if exist "%BUILD_DIR%" rd /S /Q "%BUILD_DIR%"
if exist "%COV_DIR%" rd /S /Q "%COV_DIR%"
if exist "%REPORT_DIR%" rd /S /Q "%REPORT_DIR%"

mkdir "%BUILD_DIR%"
mkdir "%COV_DIR%"
mkdir "%REPORT_DIR%"
if not exist "%HISTORY_DIR%" mkdir "%HISTORY_DIR%"

for /R %%f in (*.gcda *.gcno *.gcov) do del /Q "%%f" >nul 2>&1

echo [OK] Clean complete
echo.

REM ============================================================
REM TOOL CHECK
REM ============================================================

echo [2/5] Checking required tools...

where gcc >nul 2>&1 || (echo [ERROR] GCC not found & pause & exit /b 1)
where gcovr >nul 2>&1 || (echo [ERROR] gcovr not found & pause & exit /b 1)
where reportgenerator >nul 2>&1 || (echo [ERROR] ReportGenerator not found & pause & exit /b 1)

echo [OK] All tools found
echo.

REM ============================================================
REM Check for missing header files
REM ============================================================

echo [INFO] Checking for required headers...

if not exist "src\backend\database\alert_service.h" (
    echo [WARNING] alert_service.h not found, creating empty header...
    echo // Empty header file for compilation > src\backend\database\alert_service.h
)

if not exist "src\backend\database\db_connection.h" (
    echo [ERROR] db_connection.h not found!
    echo Please ensure src\backend\database\db_connection.h exists
    pause
    exit /b 1
)

REM ============================================================
REM Check for cJSON library
REM ============================================================

echo [INFO] Checking for cJSON library...

if not exist "C:\msys64\mingw64\lib\libcjson.a" (
    echo [WARNING] cJSON library not found in MSYS64, trying to find elsewhere...
    if not exist "src\backend\database\cJSON.c" (
        echo [ERROR] Neither cJSON library nor cJSON.c source file found!
        echo Please install cJSON: pacman -S mingw-w64-x86_64-cjson
        echo or ensure src\backend\database\cJSON.c exists
        pause
        exit /b 1
    ) else (
        echo [OK] Using cJSON.c from source
        set "USE_CJSON_SOURCE=1"
    )
) else (
    echo [OK] cJSON library found
    set "USE_CJSON_SOURCE=0"
)

echo [OK] Header check complete
echo.

REM ============================================================
REM COMMON FLAGS (Branch coverage enabled)
REM ============================================================

set "CFLAGS=-Isrc\backend -Isrc\backend\database -Isrc\backend\security -Isrc\backend\core\utils -Isrc\tests -Isrc\tests\mock_includes -IC:\msys64\mingw64\include -g -O0 --coverage"

REM Linker flags - cJSON kütüphanesini EKLEDİM!
set "LDFLAGS=--coverage -mconsole -Wl,-subsystem,console -lws2_32 -lpq -lcjson -lcmocka"

REM Eğer cJSON kaynak dosyasını kullanacaksak, ayrıca bağlamaya gerek yok
if "%USE_CJSON_SOURCE%"=="1" (
    echo [INFO] Using cJSON.c from source instead of -lcjson
    set "LDFLAGS=--coverage -mconsole -Wl,-subsystem,console -lws2_32 -lpq -lcjson -lcmocka"
)

set TEST_COUNT=0
set TEST_FAILED=0

echo [3/5] Compiling and Running All Tests...
echo.

goto RUN_TESTS

:run_test
set TEST_NAME=%1
shift

set "SOURCE_FILES="
:collect_files
if "%1"=="" goto compile_test
set "SOURCE_FILES=%SOURCE_FILES% %1"
shift
goto collect_files

:compile_test
echo ------------------------------------------------------------
echo Running: %TEST_NAME%
echo ------------------------------------------------------------

REM cJSON kaynak dosyasını gerektiğinde EKLE
if "%USE_CJSON_SOURCE%"=="1" (
    echo [INFO] Adding cJSON.c to compilation
    gcc %CFLAGS% %SOURCE_FILES% src\backend\database\cJSON.c -o "%BUILD_DIR%\%TEST_NAME%.exe" %LDFLAGS%
) else (
    gcc %CFLAGS% %SOURCE_FILES% -o "%BUILD_DIR%\%TEST_NAME%.exe" %LDFLAGS%
)

if errorlevel 1 (
    echo   [FAIL] Compilation failed
    set /a TEST_FAILED+=1
    goto :eof
)

REM Run the test
"%BUILD_DIR%\%TEST_NAME%.exe"
if errorlevel 1 (
    echo   [FAIL] Test execution failed
    set /a TEST_FAILED+=1
    goto :eof
)

echo   [OK] Passed
set /a TEST_COUNT+=1
goto :eof

:RUN_TESTS

REM --- Data Structure Tests (Console Apps) ---
call :run_test test_queue ^
src\backend\tests\unit\test_queue.c ^
src\backend\core\data_structures\queue.c

call :run_test test_stack ^
src\backend\tests\unit\test_stack.c ^
src\backend\core\data_structures\stack.c

call :run_test test_bst ^
src\backend\tests\unit\test_bst.c ^
src\backend\core\data_structures\bst.c

call :run_test test_heap ^
src\backend\tests\unit\test_heap.c ^
src\backend\core\data_structures\heap.c

call :run_test test_graph ^
src\backend\tests\unit\test_graph.c ^
src\backend\core\data_structures\graph.c

REM --- API Tests ---
REM call :run_test test_api ^
REM src\backend\tests\unit\test_api_handlers.c ^
REM src\backend\api\router.c ^
REM src\backend\api\http_server.c ^
REM src\backend\api\handlers\machine_handler.c ^
REM src\backend\api\handlers\inventory_handler.c ^
REM src\backend\api\handlers\auth_handler.c ^
REM src\backend\api\handlers\maintenance_handler.c ^
REM src\backend\api\handlers\report_handler.c ^
REM src\backend\api\handlers\fault_handler.c ^
REM src\backend\database\alert_service.c ^
REM src\backend\database\machine_service.c ^
REM src\backend\database\db_connection.c ^
REM src\backend\database\inventory_service.c ^
REM src\backend\database\maintenance_service.c ^
REM src\backend\security\jwt.c ^
REM src\backend\security\rbac.c ^
REM src\backend\security\encryption.c ^
REM src\backend\core\utils\logger.c ^
REM src\backend\core\utils\memory.c ^
REM src\backend\core\utils\time_utils.c

REM --- Individual Handler Unit Tests ---

call :run_test test_auth_handler ^
src\backend\tests\unit\test_auth_handler.c ^
src\backend\api\handlers\auth_handler.c ^
src\backend\security\jwt.c ^
src\backend\database\db_connection.c

call :run_test test_inventory_handler ^
src\backend\tests\unit\test_inventory_handler.c ^
src\backend\api\handlers\inventory_handler.c

call :run_test test_fault_handler ^
src\backend\tests\unit\test_fault_handler.c ^
src\backend\api\handlers\fault_handler.c

call :run_test test_machine_handler ^
src\backend\tests\unit\test_machine_handler.c ^
src\backend\api\handlers\machine_handler.c      

call :run_test test_report_handler ^
src\backend\tests\unit\test_report_handler.c ^
src\backend\api\handlers\report_handler.c

call :run_test test_maintenance_handler ^
src\backend\tests\unit\test_maintenance_handler.c ^
src\backend\api\handlers\maintenance_handler.c


REM --- Database Tests ---
call :run_test test_db_pool_basic ^
src\backend\tests\unit\database\test_db_pool_basic.c ^
src\backend\database\db_connection.c

call :run_test test_db_pool_init ^
src\backend\tests\unit\database\test_db_pool_init.c ^
src\backend\database\db_connection.c

call :run_test test_db_pool_fifo ^
src\backend\tests\unit\database\test_db_pool_fifo.c

REM --- Service Tests (TEST_MODE) ---

call :run_test test_inventory_service ^
src\backend\tests\unit\database\test_inventory_service.c ^
src\backend\database\inventory_service.c ^
src\backend\database\db_connection.c

call :run_test test_machine_service ^
src\backend\tests\unit\database\test_machine_service.c ^
src\backend\database\machine_service.c ^
src\backend\database\db_connection.c

call :run_test test_maintenance_service ^
src\backend\tests\unit\database\test_maintenance_service.c ^
src\backend\database\maintenance_service.c ^
src\backend\database\db_connection.c



REM --- REAL Service Tests (TEST_MODE OLMADAN) ---
call :run_test test_alert_service_real ^
src\backend\tests\unit\database\test_alert_service_real.c ^
src\backend\database\alert_service.c

call :run_test test_inventory_service_real ^
src\backend\tests\unit\database\test_inventory_service_real.c ^
src\backend\database\inventory_service.c

call :run_test test_machine_service_real ^
src\backend\tests\unit\database\test_machine_service_real.c ^
src\backend\database\machine_service.c

call :run_test test_maintenance_service_real ^
src\backend\tests\unit\database\test_maintenance_service_real.c ^
src\backend\database\maintenance_service.c

call :run_test test_db_connection_real ^
src\backend\tests\unit\database\test_db_connection_real.c ^
src\backend\database\db_connection.c

REM --- HTTP Server Tests  ---
REM  call :run_test test_http_server ^
REM  src\backend\tests\unit\test_http_server.c ^
REM  src\backend\api\http_server.c ^
REM -DUNIT_TEST



REM --- DB Connection Tests ---
call :run_test test_db_connection ^
src\backend\tests\unit\database\test_db_connection.c ^
src\backend\database\db_connection.c

REM --- Router Tests ---
call :run_test test_router ^
src\backend\tests\unit\test_router.c ^
src\backend\api\router.c

REM --- Security Tests ---
call :run_test test_rbac ^
src\backend\tests\unit\test_rbac.c ^
src\backend\security\rbac.c

call :run_test test_jwt ^
src\backend\tests\unit\test_jwt_standalone.c ^




REM --Standalone Tests ---
call :run_test test_maintenance_standalone ^
src\backend\tests\unit\test_maintenance_standalone.c

call :run_test test_machine_standalone ^
src\backend\tests\unit\test_machine_standalone.c




REM --- Integration Tests ---
call :run_test test_machine_module_int ^
src\backend\tests\integration\test_machine_module.c ^
src\backend\modules\machine\machine_service.c ^
src\backend\database\db_connection.c

echo.
echo Tests Passed: %TEST_COUNT%
echo Tests Failed: %TEST_FAILED%
echo.

REM ============================================================
REM COVERAGE XML (gcovr)
REM ============================================================

echo [4/5] Generating coverage.xml with gcovr...

gcovr ^
  -r "%ROOT%" ^
  --object-directory "%BUILD_DIR%" ^
  --xml-pretty ^
  --exclude-directories "%BUILD_DIR%" ^
   --exclude "src/backend/tests/unit/test_api_handlers.c" ^
  --exclude "C:/msys64/*" ^
  --exclude "src/backend/api/http_server.c" ^
  --merge-mode-functions=merge-use-line-0 ^
  -o "%COV_DIR%\coverage.xml"

if errorlevel 1 (
    echo [ERROR] gcovr generation failed!
    pause
    exit /b 1
)

echo [OK] coverage.xml created
echo.

REM ============================================================
REM REPORT (Classic Dashboard + History)
REM ============================================================

echo [5/5] Generating HTML report with ReportGenerator...

reportgenerator ^
    -reports:"%COV_DIR%\coverage.xml" ^
    -targetdir:"%REPORT_DIR%" ^
    -historydir:"%HISTORY_DIR%" ^
    -reporttypes:Html ^
    -title:"Backend C Tests - Coverage Report" ^
    -verbosity:Warning ^
    -filefilters:"-C:/msys64/mingw64/include/*" ^
    -filefilters:"-*/mingw64/*"

if errorlevel 1 (
    echo [ERROR] Report generation failed!
    pause
    exit /b 1
)

echo.
echo ============================================================
echo   COVERAGE REPORT GENERATED SUCCESSFULLY
echo ============================================================
echo.
echo %ROOT%\%REPORT_DIR%\index.html
echo.

start "" "%ROOT%\%REPORT_DIR%\index.html"

pause