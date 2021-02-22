#!/bin/bash

if [[ $# -ne 1 ]]
then
    echo "Usage: copy-to-pico.sh <file>"
    exit 1
fi

if [[ ! ${1: -4} == '.uf2' ]]
then
    echo "Error, not an .uf2 file."
    exit 1
fi

for i in {1..20}
do
    PICO_DEVICE=$(blkid --label RPI-RP2)

    if [[ -e "$PICO_DEVICE" ]]
    then
        break
    fi

    sleep 0.1
done

if [[ ! -e "$PICO_DEVICE" ]]
then
    echo "[copy-to-pico.sh]: Can not find Raspberry Pi Pico device."
    exit 1
fi

echo "Writing image to Raspberry Pi Pico."

sudo dd if="$1" of="$PICO_DEVICE" conv=fsync bs=4M status=none
exit $?
