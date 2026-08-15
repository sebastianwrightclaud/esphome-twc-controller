#!/bin/sh
# Host-side check of the bytes the controller puts on the wire, against the
# frame format documented in ngardiner/TWCManager. No hardware or ESPHome
# toolchain needed - test/stubs/ provides just enough of the esphome and
# FreeRTOS surface to link the real component sources.
set -e
here=$(dirname "$0")
out=${TMPDIR:-/tmp}/twc_wire_test
g++ -std=gnu++17 -w \
    -I "$here/stubs" -I "$here/../components/twc-controller" \
    -include "$here/stubs/prelude.h" \
    -o "$out" \
    "$here/wire_test.cpp" \
    "$here/../components/twc-controller/twc_protocol.cpp" \
    "$here/../components/twc-controller/twc_connector.cpp" \
    "$here/../components/twc-controller/functions.cpp"
"$out"
