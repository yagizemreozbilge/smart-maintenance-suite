#!/bin/bash

echo "Generating Coverage Report..."
# Requires lcov

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_ROOT="$SCRIPT_DIR/../../.."

# Recompile with coverage flags
gcc -g -Wall -fprofile-arcs -ftest-coverage \
    "$BACKEND_ROOT/tests/runners/run_unit_tests.c" \
    "$BACKEND_ROOT/tests/unit/"*.c \
    "$BACKEND_ROOT/core/data_structures/"*.c \
    -o "$BACKEND_ROOT/tests/coverage_runner"

# Run
"$BACKEND_ROOT/tests/coverage_runner"

# Capture
lcov --capture --directory "$BACKEND_ROOT" --output-file coverage.info
# Generate HTML
genhtml coverage.info --output-directory out
echo "Report generated in 'out' directory."
