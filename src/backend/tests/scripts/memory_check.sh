#!/bin/bash

echo "Running Memory Check (Valgrind)..."
# Check if valgrind exists
if ! command -v valgrind &> /dev/null; then
    echo "Valgrind not found. Please install it."
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_ROOT="$SCRIPT_DIR/../../../.."
TEST_BINARY="$BACKEND_ROOT/src/backend/tests/unit_test_runner"

if [ ! -f "$TEST_BINARY" ]; then
    echo "Test binary not found. Run run_tests.sh first."
    exit 1
fi

valgrind --leak-check=full --track-origins=yes "$TEST_BINARY"
