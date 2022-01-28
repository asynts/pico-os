There is a substantial amount of code that was imported from the official SDK.
All of this code was taken from git revision 2062372d203b372849d573f252cf7c6dc2800c0a:
https://github.com/raspberrypi/pico-sdk/tree/2062372d203b372849d573f252cf7c6dc2800c0a

### Boot Procedure

 1. The processor starts up and takes first two words from ROM to setup the stack and to jump to the reset handler.

 2. The ROM will load the first 256 bytes from flash into RAM at `0x20041f00`.
    It computes the CRC32 of this and compares it to a checksum at the end of this sector.
    The checksum isn't a normal CRC32 but it has been modified slightly to allow computing it faster.

 3. The ROM will jump to `0x20041f00`.
    In my understanding the stack is setup too, but I do not know where exactly.

 4. The first sector is responsible to initialize the flash properly.
    This code is almost entirely copied from the SDK.

    Afterwards, it will put `0x10000100` into the vector table and then sets up the stack according to it.
    Then it jumps to the reset hander defined in the vector table.

 5. Finally, we regain control.
    Currently, this hasn't been implemented.
