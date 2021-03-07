import invoke
import os
import tempfile

@invoke.task
def picoprobe(c, debug=False):
    """
    This script connects to a picoprobe device that is used for debugging.
    Before using this script, the device needs to be plugged in.
    """

    if debug:
        debug_flags = "--debug=3"
    else:
        debug_flags = ""

    c.sudo(f"openocd {debug_flags} -f interface/picoprobe.cfg -f target/rp2040.cfg", pty=True)

@invoke.task
def debugger(c, gdb="arm-none-eabi-gdb", port=3333):
    """
    This script connects to the debugger interface exposed by picoprobe.
    Before using this script, picoprobe has to be run.
    """

    init_script = tempfile.NamedTemporaryFile(suffix=".gdb")

    init_script.write(f"""\
target extended-remote localhost:{port}
file Kernel.elf
source ../Scripts/dynamic-loader.py

define rebuild
    shell ninja
    load
    monitor reset init
end
""".encode())
    init_script.flush()

    c.run(f"{gdb} -q -x {init_script.name}", pty=True)

@invoke.task
def messages(c):
    """
    This script connects to the serial interface exposed by picoprobe.  Before
    using this script, picoprobe has to be run.
    """

    if not os.path.exists("/dev/ttyACM0"):
        print("Can not find serial device '/dev/ttyACM0'.")
        exit(1)

    c.sudo("stty -F /dev/ttyACM0 115200 igncr")
    c.sudo("cat /dev/ttyACM0", pty=True)
