#!/bin/bash
set -e

# This script connects to the debugger interface exposed by picoprobe. Before
# use this script, the device has to be plugged in and /Scripts/picoprobe.sh
# has to be run.

gdb_script_file=$(mktemp)
cat > $gdb_script_file <<EOD
file Kernel.elf
target extended-remote localhost:3333

define rebuild
    shell ninja
    load
    monitor reset init
end
EOD

arm-none-eabi-gdb -q -ix $gdb_script_file
