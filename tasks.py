import invoke
import os
import tempfile

@invoke.task
def probe(context: invoke.context.Context, debug=False):
    if debug:
        debug_flags = "--debug=3"
    else:
        debug_flags = ""

    context.sudo(f"openocd {debug_flags} -f interface/picoprobe.cfg -f target/rp2040.cfg", pty=True, echo=True)

@invoke.task
def dbg(context: invoke.context.Context, gdb="arm-none-eabi-gdb", port=3333):
    init_script = tempfile.NamedTemporaryFile(suffix=".gdb")

    # FIXME: This is really ugly.
    init_script.write(f"""\
file Kernel.elf
target extended-remote localhost:{port}

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

set confirm off

set history save on
set history size unlimited
set history remove-duplicates 1
""".encode())
    init_script.flush()

    context.run(f"{gdb} -q -x {init_script.name}", pty=True, echo=True)

@invoke.task
def tty(context: invoke.context.Context):
    if not os.path.exists("/dev/ttyACM0"):
        print("Can not find serial device '/dev/ttyACM0'.")
        exit(1)

    context.sudo("stty -F /dev/ttyACM0 115200 igncr", echo=True)
    context.sudo("tio /dev/ttyACM0", pty=True, echo=True)

@invoke.task
def backup(context: invoke.context.Context):
    context.run("~/dev/scripts/backup.rb --name 'pico-os' --url 'git@github.com:asynts/os' --upload 's3://backup.asynts.com/git/pico-os'", echo=True)
