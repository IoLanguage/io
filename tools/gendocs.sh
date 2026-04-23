#!/bin/sh
# Generate Io documentation from source code.
#
# Two separate trees are produced:
#   docs/Reference                                          (Io-visible API): from /*doc ...*/ and /*metadoc ...*/
#   docs/Implementation/Reference                           (C implementation): from /*cdoc ...*/
#
# Each tree gets a colvmn-style landing page plus standalone class-doc
# pages per proto. After docs2html.io writes the raw files, we run
# colvmn/static-gen.js to bake the _index.md → index.html.
#
# Usage: tools/gendocs.sh [base-output-dir]
#   base-output-dir defaults to docs

set -e

cd "$(dirname "$0")/.."

IO_WASM="${IO_WASM:-build/bin/io_static}"

if [ ! -f "$IO_WASM" ]; then
	echo "error: $IO_WASM not found. Run 'make' first." >&2
	exit 1
fi

BASE="${1:-docs}"
REF_DIR="$BASE/Reference"
INT_DIR="$BASE/Implementation/Reference"
mkdir -p "$REF_DIR" "$INT_DIR"

echo "Generating Io Reference → $REF_DIR ..."
wasmtime --dir=. --dir=/tmp "$IO_WASM" tools/io/docs2html.io "$REF_DIR" docs

echo "Generating Implementation Reference → $INT_DIR ..."
wasmtime --dir=. --dir=/tmp "$IO_WASM" tools/io/docs2html.io "$INT_DIR" cdocs

echo "Baking HTML via colvmn/static-gen.js ..."
node colvmn/static-gen.js

echo "Done."
