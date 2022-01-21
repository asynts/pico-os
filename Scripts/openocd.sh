#!/bin/bash
set -e

if [[ "$PWD" != "/home/me/dev/pico-os" ]]
then
    echo "error: script executed in unknown location"
    exit 1
fi

sudo openocd -f interface/picoprobe.cfg -f target/rp2040.cfg -s tcl
