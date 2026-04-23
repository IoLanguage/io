#!/bin/sh
# Generate Io documentation from source code.
#
# Two separate trees are produced:
#   - Reference  (Io-visible API): from /*doc ...*/ and /*metadoc ...*/
#   - Internals  (C implementation): from /*cdoc ...*/
#
# Usage: tools/gendocs.sh [base-output-dir]
#   base-output-dir defaults to docs
#   → writes $base/reference/ and $base/internals/

set -e

cd "$(dirname "$0")/.."

IO_WASM="${IO_WASM:-build/bin/io_static}"

if [ ! -f "$IO_WASM" ]; then
	echo "error: $IO_WASM not found. Run 'make' first." >&2
	exit 1
fi

BASE="${1:-docs}"
REF_DIR="$BASE/reference"
INT_DIR="$BASE/internals"
mkdir -p "$REF_DIR" "$INT_DIR"

echo "Generating Io Reference → $REF_DIR ..."
wasmtime --dir=. --dir=/tmp "$IO_WASM" tools/io/docs2html.io "$REF_DIR" docs

echo "Generating C Internals → $INT_DIR ..."
wasmtime --dir=. --dir=/tmp "$IO_WASM" tools/io/docs2html.io "$INT_DIR" cdocs

echo "Done."
