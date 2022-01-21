In order to properly debug this operating system, we need to run on real hardware.

### Installation

 1. First, we need to build `openocd`:

    ```none
    cd ~/src/github.com/raspberrypi
    git clone https://github.com/raspberrypi/openocd.git --recursive --branch rp2040 --depth=1
    ./bootstrap
    CFLAGS="-Wno-misleading-indentation -Wno-stringop-overflow" ./configure
    make -j20
    sudo make install
    ```

### Debugging

 1. We plug in the Raspberry Pi Pico which has been wired for the Picoprobe setup.

 2. We start the `openocd` server with:

    ```none
    ./Scripts/openocd.sh
    ```

 3. We start the debugger with:

    ```none
    ./Scripts/debugger.sh
    ```
