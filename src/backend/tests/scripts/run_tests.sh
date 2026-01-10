#!/bin/bash

# Navigate to project root (assuming script runs from src/backend/tests/scripts or we force paths)
# Let's assume we run this from src/backend/tests/ or project root. 
# Best practice: Find root relative to script.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR/../../../.."
# Adjust path to backend root
BACKEND_ROOT="$PROJECT_ROOT/src/backend"

echo "Compiling Unit Tests..."

gcc -g -Wall \
    "$BACKEND_ROOT/tests/runners/run_unit_tests.c" \
    "$BACKEND_ROOT/tests/unit/"*.c \
    "$BACKEND_ROOT/core/data_structures/"*.c \
    -o "$BACKEND_ROOT/tests/unit_test_runner"

if [ $? -eq 0 ]; then
    echo "Compilation Successful. Running Tests..."
    "$BACKEND_ROOT/tests/unit_test_runner"
else
    echo "Compilation Failed!"
    exit 1
fi
