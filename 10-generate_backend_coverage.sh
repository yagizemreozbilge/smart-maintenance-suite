#!/bin/bash

# ============================================================
#   BACKEND GCC COVERAGE - WSL/LINUX FULL TEST SUITE
#   (Isolated Build Version for Parallel Runs)
# ============================================================

set -e

ROOT="$(pwd)"
BUILD_DIR="$ROOT/build_backend_tests_linux"
COV_DIR="$ROOT/coverage_backend_linux"
REPORT_DIR="$ROOT/docs/coverage_backend_report_linux"
HISTORY_DIR="$ROOT/docs/coverage_history_linux"

echo "============================================================"
echo "  BACKEND GCC COVERAGE - FULL TEST SUITE (WSL ADAPTED)"
echo "============================================================"
echo ""

# ============================================================
# CLEAN (Only Linux specific dirs)
# ============================================================
echo "[1/5] Cleaning old build and coverage data..."
rm -rf "$BUILD_DIR" "$COV_DIR" "$REPORT_DIR"
mkdir -p "$BUILD_DIR" "$COV_DIR" "$REPORT_DIR" "$HISTORY_DIR"

echo "[OK] Clean complete (Isolated Linux)"
echo ""

# ============================================================
# TOOL CHECK
# ============================================================
echo "[2/5] Checking required tools..."
command -v gcc >/dev/null 2>&1 || { echo "[ERROR] gcc not found"; exit 1; }
command -v gcovr >/dev/null 2>&1 || { echo "[ERROR] gcovr not found"; exit 1; }
if ! command -v reportgenerator >/dev/null 2>&1; then
    export PATH="$PATH:$HOME/.dotnet/tools"
fi

# ============================================================
# COMMON FLAGS
# ============================================================
CFLAGS="-Isrc/backend -Isrc/backend/database -Isrc/backend/security -Isrc/backend/core/utils -Isrc/tests -Isrc/tests/mock_includes -I/usr/include/postgresql -g -O0 --coverage -fcommon -pthread"
LDFLAGS="--coverage -lpq -lcjson -lcmocka -lm -pthread"

TEST_COUNT=0
TEST_FAILED=0

# ============================================================
# RUN TEST FUNCTION (Isolated Objects)
# ============================================================
run_test() {
    local test_name=$1
    shift
    local source_files=$@

    echo "------------------------------------------------------------"
    echo "Running: $test_name"
    echo "------------------------------------------------------------"

    # We compile to objects first to FORCE .gcno and .gcda into BUILD_DIR
    # This prevents root pollution and N/A errors
    local obj_files=""
    for src in $source_files; do
        filename=$(basename -- "$src")
        # Unique object name to avoid clashes between tests sharing source files
        obj_file="$BUILD_DIR/${test_name}_${filename%.*}.o"
        gcc $CFLAGS -c "$src" -o "$obj_file"
        obj_files="$obj_files $obj_file"
    done

    # Link the objects to the final executable
    gcc $obj_files -o "$BUILD_DIR/$test_name" $LDFLAGS
    
    if [ $? -ne 0 ]; then
        echo "  [FAIL] Link failed"
        TEST_FAILED=$((TEST_FAILED + 1))
        return 1
    fi

    # Run the test binary
    "$BUILD_DIR/$test_name"
    
    if [ $? -ne 0 ]; then
        echo "  [FAIL] Test execution failed"
        TEST_FAILED=$((TEST_FAILED + 1))
    else
        echo "  [OK] Passed"
        TEST_COUNT=$((TEST_COUNT + 1))
    fi
}

echo "[3/5] Compiling and Running All Tests..."
echo ""

# RUN TESTS (IDENTICAL SEQUENCE TO .BAT)
run_test test_queue src/backend/tests/unit/test_queue.c src/backend/core/data_structures/queue.c
run_test test_stack src/backend/tests/unit/test_stack.c src/backend/core/data_structures/stack.c
run_test test_bst src/backend/tests/unit/test_bst.c src/backend/core/data_structures/bst.c
run_test test_heap src/backend/tests/unit/test_heap.c src/backend/core/data_structures/heap.c
run_test test_graph src/backend/tests/unit/test_graph.c src/backend/core/data_structures/graph.c
run_test test_auth_handler src/backend/tests/unit/test_auth_handler.c src/backend/api/handlers/auth_handler.c src/backend/security/jwt.c src/backend/database/db_connection.c
run_test test_inventory_handler src/backend/tests/unit/test_inventory_handler.c src/backend/api/handlers/inventory_handler.c
run_test test_fault_handler src/backend/tests/unit/test_fault_handler.c src/backend/api/handlers/fault_handler.c
run_test test_machine_handler src/backend/tests/unit/test_machine_handler.c src/backend/api/handlers/machine_handler.c
run_test test_report_handler src/backend/tests/unit/test_report_handler.c src/backend/api/handlers/report_handler.c
run_test test_maintenance_handler src/backend/tests/unit/test_maintenance_handler.c src/backend/api/handlers/maintenance_handler.c
run_test test_db_pool_basic src/backend/tests/unit/database/test_db_pool_basic.c src/backend/database/db_connection.c
run_test test_db_pool_init src/backend/tests/unit/database/test_db_pool_init.c src/backend/database/db_connection.c
run_test test_db_pool_fifo src/backend/tests/unit/database/test_db_pool_fifo.c
run_test test_inventory_service src/backend/tests/unit/database/test_inventory_service.c src/backend/database/inventory_service.c src/backend/database/db_connection.c
run_test test_machine_service src/backend/tests/unit/database/test_machine_service.c src/backend/database/machine_service.c src/backend/database/db_connection.c
run_test test_maintenance_service src/backend/tests/unit/database/test_maintenance_service.c src/backend/database/maintenance_service.c src/backend/database/db_connection.c
run_test test_alert_service_real src/backend/tests/unit/database/test_alert_service_real.c src/backend/database/alert_service.c
run_test test_inventory_service_real src/backend/tests/unit/database/test_inventory_service_real.c src/backend/database/inventory_service.c
run_test test_machine_service_real src/backend/tests/unit/database/test_machine_service_real.c src/backend/database/machine_service.c
run_test test_maintenance_service_real src/backend/tests/unit/database/test_maintenance_service_real.c src/backend/database/maintenance_service.c
run_test test_db_connection_real src/backend/tests/unit/database/test_db_connection_real.c src/backend/database/db_connection.c
run_test test_db_connection src/backend/tests/unit/database/test_db_connection.c src/backend/database/db_connection.c
run_test test_router src/backend/tests/unit/test_router.c src/backend/api/router.c src/backend/database/db_connection.c src/backend/security/jwt.c
run_test test_rbac src/backend/tests/unit/test_rbac.c src/backend/security/rbac.c
run_test test_logger src/backend/tests/unit/test_logger.c src/backend/core/utils/logger.c
run_test test_jwt src/backend/tests/unit/test_jwt_standalone.c src/backend/security/jwt.c src/backend/database/db_connection.c
run_test test_maintenance_standalone src/backend/tests/unit/test_maintenance_standalone.c
run_test test_machine_standalone src/backend/tests/unit/test_machine_standalone.c
run_test test_machine_module_int src/backend/tests/integration/test_machine_module.c src/backend/modules/machine/machine_service.c src/backend/database/db_connection.c

echo ""
echo "Tests Passed: $TEST_COUNT"
echo "Tests Failed: $TEST_FAILED"
echo ""

# ============================================================
# COVERAGE XML (gcovr)
# ============================================================
echo "[4/5] Generating coverage.xml with gcovr..."

# We point directly to the Linux build dir for coverage data
gcovr \
  -r "$ROOT" \
  --object-directory "$BUILD_DIR" \
  --xml-pretty \
  --exclude-directories "build_backend_tests" \
  --exclude-directories "coverage_backend" \
  --exclude "src/backend/tests/unit/test_api_handlers.c" \
  --exclude "/usr/*" \
  --exclude "src/backend/api/http_server.c" \
  --print-summary \
  -o "$COV_DIR/coverage.xml"

echo "[OK] coverage.xml created"
echo ""

# ============================================================
# REPORT
# ============================================================
echo "[5/5] Generating HTML report with ReportGenerator..."
reportgenerator \
    -reports:"$COV_DIR/coverage.xml" \
    -targetdir:"$REPORT_DIR" \
    -historydir:"$HISTORY_DIR" \
    -reporttypes:Html \
    -title:"Backend C Tests - Coverage Report (WSL)" \
    -verbosity:Warning

echo ""
echo "============================================================"
echo "   COVERAGE REPORT GENERATED SUCCESSFULLY"
echo "   Report: $REPORT_DIR/index.html"
echo "============================================================"
