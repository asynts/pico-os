import invoke
import os
import tempfile

@invoke.task
def picoprobe(c, debug=False):
    if debug:
        debug_flags = "--debug=3"
    else:
        debug_flags = ""

    c.sudo(f"openocd {debug_flags} -f interface/picoprobe.cfg -f target/rp2040.cfg", pty=True)

@invoke.task
def debugger(c, gdb="arm-none-eabi-gdb", port=3333):
    init_script = tempfile.NamedTemporaryFile(suffix=".gdb")

    # FIXME: This is really ugly.
    init_script.write(f"""\
target extended-remote localhost:{port}
exec-file Kernel.1.elf
symbol-file Kernel.2.elf

define dis_here
    x/20i ($pc -20)
end

define si_and_dis
    si
    dis_here
end

define rebuild
    shell ninja
    load
    monitor reset init
end

source ../Scripts/dynamic-loader.py
""".encode())
    init_script.flush()

    c.run(f"{gdb} -q -x {init_script.name}", pty=True)

@invoke.task
def messages(c):
    if not os.path.exists("/dev/ttyACM0"):
        print("Can not find serial device '/dev/ttyACM0'.")
        exit(1)

    c.sudo("stty -F /dev/ttyACM0 115200 igncr")
    c.sudo("tio /dev/ttyACM0", pty=True)
