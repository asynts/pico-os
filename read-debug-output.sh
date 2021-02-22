#!/bin/bash
set -e
set -m

for i in {1..20}
do
    if [[ -e /dev/ttyACM0 ]]
    then
        break
    fi

    sleep 0.1
done

if [[ ! -e /dev/ttyACM0 ]]
then
    echo "[read-debug-output.sh]: Can not find Raspberry Pi Pico device."
    exit 1
fi

echo "Reading output from device:"

echo -ne '\033[1;36m'
sudo cat /dev/ttyACM0 | awk 'NF > 0' &

echo "connect" | sudo tee /dev/ttyACM0 > /dev/null

fg %1 > /dev/null
