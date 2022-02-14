### Boot Procedure

Originally, I wrote the boot loader myself, however, this is simply a waste of time.
There are so many things that require deep insight and that are very poorly documented.

I do have a pretty good understanding, how the boot loader works:

 1. The processor starts up and takes first two words from ROM to setup the stack and to jump to the reset handler.

 2. The ROM will load the first 256 bytes from flash into RAM at `0x20041f00`.
    It computes the CRC32 of this and compares it to a checksum at the end of this sector.
    The checksum isn't a normal CRC32 but it has been modified slightly to allow computing it faster.

 3. The ROM will jump to `0x20041f00`.
    In my understanding the stack is setup too, but I do not know where exactly.

 4. The first sector is responsible to initialize the flash properly.
    This is what I am importing from the pico-sdk project.

    Afterwards, it will load the vector table from `0x10000100` and then calls the reset vector with the stack pointer
    that is provided in the vector table.

 5. This is where we first gain control.

### Obtaining the Boot Loader

 1. Create a new directly to work in:

    ```bash
    cd $(mktemp -d)
    ```

 2. Clone the `pico-sdk` project:

    ```bash
    git clone ~/src/github.com/raspberrypi/pico-sdk
    ```

 3. Create a `CMakeLists.txt` file:

    ```cmake
    cmake_minimum_required(VERSION 3.13)

    # initialize pico-sdk from submodule
    # note: this must happen before project()
    include(pico-sdk/pico_sdk_init.cmake)

    project(my_project)

    # initialize the Raspberry Pi Pico SDK
    pico_sdk_init()

    # rest of your project
    ```

 4. Build the project:

    ```bash
    cdm build
    cmake .. -GNinja
    ninja
    ```

 5. Extract the boot loader:

    ```bash
    cp pico-sdk/src/rp2_common/boot_stage2/bs2_default.elf ~/dev/pico-os/Extern/boot.elf
    ```
