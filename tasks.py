"""
Pico-RTOS Development Tasks

Usage:
    inv probe      - Start OpenOCD debugger
    inv dbg        - Start GDB session
    inv tty        - Connect to serial console
    inv backup     - Backup repository
    inv flash      - Flash firmware via UF2
"""

import invoke
import os
import tempfile
import glob

# GDB initialization script content
GDB_INIT_SCRIPT = """\
target extended-remote localhost:{port}
file build/Kernel.elf

define dis_here
    x/20i ($pc -20)
end

define si_and_dis
    si
    dis_here
end

define rebuild
    shell ninja -C build
    load
    monitor reset init
end

set confirm off
set history save on
set history size unlimited
set history remove-duplicates 1
"""


@invoke.task
def probe(c, debug=False):
    """Start OpenOCD with PicoProbe interface."""
    debug_flags = "--debug=3" if debug else ""
    c.sudo(f"openocd {debug_flags} -f interface/picoprobe.cfg -f target/rp2040.cfg", pty=True)


@invoke.task
def dbg(c, gdb="arm-none-eabi-gdb", port=3333):
    """Start GDB session connected to OpenOCD."""
    with tempfile.NamedTemporaryFile(mode='w', suffix=".gdb", delete=False) as init_script:
        init_script.write(GDB_INIT_SCRIPT.format(port=port))
        init_script.flush()
        c.run(f"{gdb} -q -x {init_script.name}", pty=True)


@invoke.task
def tty(c):
    """Connect to serial console (supports Linux and macOS)."""
    # Try common serial device paths
    linux_devices = ["/dev/ttyACM0", "/dev/ttyACM1", "/dev/ttyUSB0"]
    macos_devices = glob.glob("/dev/cu.usbmodem*") + glob.glob("/dev/tty.usbmodem*")

    device = None
    for path in linux_devices + macos_devices:
        if os.path.exists(path):
            device = path
            break

    if device is None:
        print("Error: No serial device found.")
        print("Checked: /dev/ttyACM*, /dev/ttyUSB*, /dev/cu.usbmodem*, /dev/tty.usbmodem*")
        exit(1)

    print(f"Connecting to {device}...")

    if device.startswith("/dev/cu") or device.startswith("/dev/tty."):
        # macOS
        c.run(f"screen {device} 115200", pty=True)
    else:
        # Linux
        c.sudo(f"stty -F {device} 115200 igncr")
        c.sudo(f"tio {device}", pty=True)


@invoke.task
def flash(c, uf2_path="build/Kernel.1.uf2"):
    """Flash firmware via UF2 (Pico must be in BOOTSEL mode)."""
    # Check for mounted Pico
    mount_points = ["/Volumes/RPI-RP2", "/media/*/RPI-RP2", "/run/media/*/RPI-RP2"]

    pico_mount = None
    for pattern in mount_points:
        matches = glob.glob(pattern)
        if matches:
            pico_mount = matches[0]
            break

    if pico_mount is None:
        print("Error: Pico not found in BOOTSEL mode.")
        print("Hold BOOTSEL while connecting USB, then retry.")
        exit(1)

    print(f"Flashing {uf2_path} to {pico_mount}...")
    c.run(f"cp {uf2_path} {pico_mount}/")
    print("Done! Pico will reboot automatically.")


@invoke.task
def backup(c):
    """Backup repository to S3."""
    c.run("~/dev/scripts/backup.rb --name 'pico-os' --url 'git@github.com:asynts/os' --upload 's3://backup.asynts.com/git/pico-os'")
