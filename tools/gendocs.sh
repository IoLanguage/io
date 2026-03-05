#!/bin/sh
# Generate Io reference documentation from source code.
# Parses //doc, /*doc, and metadoc comments from C and Io files,
# then generates HTML pages.
#
# Usage: tools/gendocs.sh [output-dir]
#   output-dir defaults to docs/reference

set -e

cd "$(dirname "$0")/.."

IO_WASM="${IO_WASM:-build/bin/io_static}"

if [ ! -f "$IO_WASM" ]; then
	echo "error: $IO_WASM not found. Run 'make' first." >&2
	exit 1
fi

OUTDIR="${1:-docs/reference}"
mkdir -p "$OUTDIR"

echo "Extracting docs..."
wasmtime --dir=. --dir=/tmp "$IO_WASM" tools/io/docs2html.io "$OUTDIR"

echo "Done. Output in $OUTDIR"
