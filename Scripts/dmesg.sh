#!/bin/bash
set -e

# This script connects to the serial interface exposed by picoprobe.  Before
# use this script, the device has to be plugged in and /Scripts/picoprobe.sh
# has to be run.

if [[ ! -e /dev/ttyACM0 ]]
then
    echo "Can not find Raspberry Pi Pico device."
    exit 1
fi

sudo socat stdio /dev/ttyACM0
