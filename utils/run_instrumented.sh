#!/bin/bash
# Instrumentation and execution wrapper for memprof
# Usage: ./run_instrumented.sh <source_dir> <binary_name> [compiler_args...]

set -e

if [ $# -lt 2 ]; then
    echo "Usage: $0 <source_directory> <binary_name> [compiler_args...]"
    exit 1
fi

SOURCE_DIR="$1"
BINARY_NAME="$2"
shift 2
COMPILER_ARGS="$@"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

# Check if source directory exists
if [ ! -d "$SOURCE_DIR" ]; then
    echo "Error: Source directory '$SOURCE_DIR' does not exist"
    exit 1
fi

# Step 1: Instrument the source code in place
echo "=========================================="
echo "Step 1: Instrumenting source code (in place)..."
echo "=========================================="

if ! python3 "$SCRIPT_DIR/instrument.py" "$SOURCE_DIR"; then
    echo "Error: Instrumentation failed"
    exit 1
fi

# Step 2: Compile the instrumented code
echo ""
echo "=========================================="
echo "Step 2: Compiling instrumented code..."
echo "=========================================="

# Find all C files in source directory
C_FILES=$(find "$SOURCE_DIR" -type f -name "*.c" | tr '\n' ' ')

if [ -z "$C_FILES" ]; then
    echo "Error: No C files found in source directory"
    exit 1
fi

# Build compile command with memprof includes
COMPILE_CMD="gcc -I$ROOT_DIR/inc $COMPILER_ARGS $C_FILES $ROOT_DIR/utils/tracker.c -o $BINARY_NAME"

echo "Compile command: $COMPILE_CMD"
$COMPILE_CMD

if [ ! -f "$BINARY_NAME" ]; then
    echo "Error: Compilation failed"
    exit 1
fi

echo "Binary created: $BINARY_NAME"

# Step 3: Run the binary with memprof
echo ""
echo "=========================================="
echo "Step 3: Running with memprof..."
echo "=========================================="

# Build the LD_PRELOAD library if needed
WRAPPER_SO="$ROOT_DIR/utils/wrapper.so"
if [ ! -f "$WRAPPER_SO" ]; then
    echo "Building wrapper.so..."
    gcc -shared -fPIC -I"$ROOT_DIR/inc" "$ROOT_DIR/utils/wrapper.c" -o "$WRAPPER_SO" -ldl
fi

# Run with memprof - you would normally run: ./memprof <binary>
# This script just handles the instrumentation and compilation
export LD_PRELOAD="$WRAPPER_SO"
export MEMPROF_PIPE="/tmp/memprof_pipe"

echo "Binary can be run with:"
echo "  LD_PRELOAD=$WRAPPER_SO ./$BINARY_NAME"
echo ""
echo "Or with memprof:"
echo "  ./memprof ./$BINARY_NAME"

# Step 4: Reminder
echo ""
echo "=========================================="
echo "Instrumentation note"
echo "=========================================="
echo "Source files were modified in place with TRACK_ASSIGN insertions."
echo "Revert your source (git checkout) if you want to undo instrumentation."
echo ""
