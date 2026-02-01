commitid 0411690acbad429b547ba2256ea25b1074d2f573

### Notes

-   I am trying to find something that can replace the cable mess for pico-os development

-   This board is interesting:
    https://www.waveshare.com/rp2350-geek.htm?srsltid=AfmBOoqbGVsM4GBO9B7neGbrInKVBdearFHvM762YZDnvCAcN2fB_oMu

    -   Unfortunately, it does not supply a power cable.

-   This is interesting:
    https://github.com/neotron-Compute/neotron-Pico

    -   This takes it a lot further than what I want.

-   I think it would be cool to design my own PCB.

    -   I would then solder both chips manually.

    -   I could do a panel, where some of them has a breakout board.

    -   The alternative would be to manually solder it.

-   I am thinking about a switch module:

    -   I can't use the debug probe, because it's not suitable for soldering directly.

    -   I can use a pico-1 as debug probe.

    -   Then I can mount a pico-1 and pico-2

    -   They are hooked up to a bus chip to ensure that only a single wire is active:
        https://www.ti.com/lit/ds/symlink/sn74cbt16211a.pdf?ts=1769547076206

-   I did ask on the Raspberry Pi forum:

    -   This seems to be exactly what I want:
        https://github.com/NevynUK/PicoDebugger

    -   I can use a different chip as the debugger which seems really cool:
        https://thepihut.com/products/seeed-xiao-rp2040

        -   I need to make sure the pin layout makes sense.

-   Setup:

    -   Create two instances of the same board, one with pico-1 another with pico-2
        This is much simpler than a bus setup.
        I could even connect both to the same computer without any issues.

    -   I can use a custom pico-1 variant as the debugger to minimize the footprint:
        https://thepihut.com/products/seeed-xiao-rp2040?variant=53975380263297

    -   I can use a custom pico-2 variant to minimize the footprint further:
        https://thepihut.com/products/xiao-rp2350-raspberry-pi-rp2350?variant=53975452123521

    -   SD card board:
        https://thepihut.com/products/adafruit-microsd-card-bff-add-on-for-qt-py-and-xiao

-   I am designing the circuit in KiCAD:

    -   I found this library which contains the custom components:
        https://github.com/Seeed-Studio/OPL_Kicad_Library

    -   Most of the components do not have a pin-out for some reason? I found this one:
        https://thepihut.com/products/adafruit-microsd-card-breakout-board

        -   That would not work because it requires 5V

    -   This one should work:
        https://thepihut.com/products/pololu-breakout-board-for-microsd-cards

    -   This should work for the HDMI port:
        https://thepihut.com/products/adafruit-dvi-breakout-board-for-hdmi-source-devices
        Unfortunately, it's sold out

    -   This could be a suitable alternative:
        https://thepihut.com/products/adafruit-dvi-sock-for-pico-works-with-hdmi-displays

-   I am unsure if I want to use JTAG or SWD

    -   I don't know if JTAG is even supported?

    -   I found this about JTAG and SWD:
        https://www.ti.com/lit/wp/spmy004/spmy004.pdf

    -   Seems that SDW is just better and JTAG is meant for other purposes

-   The HDMI stuff might not be possible at all
    https://github.com/Wren6991/PicoDVI

    -   There should be resistors between connector and chip

    -   He was overclocking the system

    -   It seems extremely complicated

    -   Maybe I can just use VGA instead, that should be less complex

-   When I order the boards, I could order multiple variants:

    -   One with header pins for initial debugging

    -   One for a regular Pico if the custom solution doesn't work

-   Some general thoughts:

    -   Both USB ports should be accessible

-   I am trying to figure out what is required for VGA text-mode:

    -   https://codaris.github.io/picovga-cmake/

    -   There seems to be an official example as well:
        https://github.com/raspberrypi/pico-playground/tree/master/scanvideo/textmode

        -   They use a lot more pins though:
            https://github.com/raspberrypi/pico-playground/blob/49a701a955ada550f8e7221aadef354eb9ff7b62/scanvideo/Raspberry%20Pi%20Pico%20to%20VGA%20Connection%20Schematic.png

    -   Here is a more detailed explanation:
        https://github.com/Megatokio/kilipili/wiki/Your-first-own-VGA-Board

        -   It's possible to use a single pin per color component with RGB111

    -   I found this but it's not quite what I want:
        https://www.amazon.de/Adapter-Raspberry-Solution-monitor-Similar/dp/B01M0BTURR

    -   This seems to be exactly what I want:
        https://www.mouser.de/ProductDetail/TE-Connectivity-AMP/3-338168-2?qs=8aIgir6HTzPF0%252BT89ZzTow%3D%3D

-   In theory, it should be possible to do VGA text mode with a shift-register, right?

    -   I am an idiot, the text mode is part of the computer, not part of the monitor.
        That is something I need to do myself, or I would have to use a separate chip.

-   Is the chip fast enough with shift registers?

    -   I want 640 x 480 white output as bare minimum

    -   It seems that a `RAMDAC` is what I am looking for?
        https://en.wikipedia.org/wiki/RAMDAC

-   I am looking at the video from BenEater again:
    https://www.youtube.com/watch?v=l7rce6IQDWs

    -   This is the pin-out he used:
        https://pinoutguide.com/Video/VGAVesaDdc_pinout.shtml

    -   It seems that the DDC pins are used to detect the monitor:
        https://en.wikipedia.org/wiki/Display_Data_Channel

    -   The keyword seems to be "vga signal timings":
        https://martin.hinner.info/vga/timing.html

        -   If I understand this correctly, you need to provide the output with 25.175 MHz

        -   That is below the 48 MHz of the chip, but means that no shift registers can be used.

-   I am trying to figure out how to get a keyboard hooked up:

    -   It seems that you need 5V for PS/2?
        https://en.wikipedia.org/wiki/PS/2_port

    -   This seems helpful once I figure out the 5V problem:
        https://forums.raspberrypi.com/viewtopic.php?t=329630

    -   This is a very simple IC2 keyboard, but I think that sucks:
        https://thepihut.com/products/m5stack-cardkb-mini-keyboard-programmable-unit-v1-1-mega8a

    -   This is probably the kind of keyboard I want to use?
        https://thepihut.com/products/miniature-keyboard-microcontroller-friendly-ps-2-and-usb

    -   This is the other way around, UARR to USB:
        https://thepihut.com/products/adafruit-ch9328-uart-to-hid-keyboard-breakout

    -   It seems to be possible to use the 5V from the USB connector directly?
        https://forums.raspberrypi.com/viewtopic.php?t=366278

    -   This video explains the PS/2 keyboard from pico:
        https://www.youtube.com/watch?v=4d3idkHB4KQ

-   I am reading about VBUS and VSYS:

    -   It seems that the GPIO24 is used to tell if VBUS is present?
        That seems to be controlled by the hardware itself.

    -   VBUS is the input power from USB which is 5V

    -   VSYS is the input power when you don't power from USB

-   I am checking if there are sufficient PIO and DMA devices:

    -   Required:

        -   VGA requires 1 PIO and 3 DMA

        -   UART requires 1 DMA

        -   PS/2 requires 1 PIO

        -   SD requires 2 DMA?

    -   Available:

        -   2 PIO (more available on RP2350)

        -   12 DMA

    -   This should work

-   It seems that there are "level shifters" that do exactly what I want:

    -   https://www.ti.com/product/CD40109B

    -   This could also work?
        https://www.mouser.de/ProductDetail/Texas-Instruments/TXB0104DR

-   I need to calculate the transistors and resistors:

    -   This is useful for transistors:
        https://www.guitarscience.net/calcs/ceswtch.htm

    -   This seems like vibe coding, I don't know enough about the basics to understand what people talk about:
        https://tinyurl.com/y8r6n9zm
        https://tinyurl.com/chhf52tb

    -   I found this video, which explains the basics:
        https://www.youtube.com/watch?v=sTu3LwpF6XI

        -   The current from base to emitter allows a much larger current to flow from the collector to emitter.

        -   Unfortunately, this doesn't talk about how to choose the transistors and resistors.

    -   It seems that we can use 300mA in total:
        https://raspberrypi.stackexchange.com/a/138358/137752

        -   I would say that 10mA per transistor is the most

    -   It seems that it's not necessary to use transistors at all, since >= 2.4V is considered high
        Low is 0.0V-0.7V and high is 2.4V-5.5V
        https://github.com/tmk/tmk_keyboard/wiki/IBM-PC-AT-Keyboard-Protocol#clock-and-data-signals

    -   It seems that the pico can handle
        Input 1.8V-3.3V range

### Tasks

-   Schematic:

    -   Add status led with shift register?

### Deferred Tasks

-   PCB:

    -   The footprint for pico should be compatible with pico-1, pico-1-w, pico-2 and pico-2-w

        -   This is especially relevant for the debug pins

    -   Find a suitable breakout board and a container for it.

        -   The size should probably be equivalent to a regular Raspberry computer?

-   Need to patch the debugprobe project with pin numbers:
    https://github.com/raspberrypi/debugprobe

    -   Create a pull request to make this configurable?

    -   Maybe workaround is possible by defining `BOARD_PICO_H_` manually?

    -   It's possible to do this using `--include=my-custom-config.h` then it can define the guard
