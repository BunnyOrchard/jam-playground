#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# The Eno web app: the engine (firmware/src/dsp/eno.c, as on the FM-1) to WebAssembly, and the page.
#   web/app/build.sh  ->  build/app/{index.html, worklet.js, eno.wasm}: a static folder, servable as is
set -e
cd "$(dirname "$0")/../.."
mkdir -p build/app
emcc -O2 -ffp-contract=off -std=gnu99 -Wall -Wextra -Wno-unused-function \
    -DEN_HOST -Ifirmware/src/dsp \
    --no-entry -sSTANDALONE_WASM -sSTACK_SIZE=262144 -sINITIAL_MEMORY=8388608 -sFILESYSTEM=0 \
    -o build/app/eno.wasm web/app/eno_api.c firmware/src/dsp/eno.c
cp web/app/index.html web/app/worklet.js build/app/
echo "app: build/app ($(wc -c < build/app/eno.wasm) B wasm)"
