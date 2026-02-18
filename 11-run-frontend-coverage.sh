#!/bin/bash

# ============================================================
#   FRONTEND REACT TEST COVERAGE (Linux/WSL Version)
# ============================================================

set -e # Hata durumunda scripti durdur

ROOT_DIR="$(pwd)"
FRONTEND_DIR="$ROOT_DIR/src/frontend"
COV_DIR="$FRONTEND_DIR/coverage_linux"
REPORT_DIR="$ROOT_DIR/docs/coverage_frontend_report_linux"

echo "Current Directory: $(pwd)"

# Tool checks
if ! command -v npm &> /dev/null; then
    echo "[ERROR] npm not found! Please install Node.js."
    exit 1
fi

if ! command -v reportgenerator &> /dev/null; then
    echo "[INFO] reportgenerator not found in PATH, checking dotnet tools..."
    export PATH="$PATH:$HOME/.dotnet/tools"
    if ! command -v reportgenerator &> /dev/null; then
        echo "[ERROR] ReportGenerator not found! Install with: dotnet tool install -g dotnet-reportgenerator-globaltool"
        exit 1
    fi
fi

echo "[1/3] Cleaning old reports..."
rm -rf "$COV_DIR"
mkdir -p "$REPORT_DIR"

echo "[2/3] Running Tests and Coverage..."
cd "$FRONTEND_DIR"
npm install
npm run test:coverage -- --coverage.reportsDirectory="coverage_linux"

echo "[3/3] Generating Report..."
cd "$ROOT_DIR"
reportgenerator \
    -reports:"$COV_DIR/lcov.info" \
    -targetdir:"$REPORT_DIR" \
    -reporttypes:Html \
    -sourcedirs:"src/frontend" \
    -title:"Frontend Coverage Report (WSL)"

echo "============================================================"
echo "   PROCESS COMPLETED SUCCESSFULLY"
echo "   Report: $REPORT_DIR/index.html"
echo "============================================================"
