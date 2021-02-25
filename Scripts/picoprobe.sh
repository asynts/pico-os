#!/bin/bash
set -e

# This script connects to a picoprobe device that is used to debug
# a Raspberry Pi Pico device via SWD.  Before running this script,
# the device needs to be plugged in.

sudo openocd -f interface/picoprobe.cfg -f target/rp2040.cfg
