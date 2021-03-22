# FIXME: This is really ugly, I really want that watchpoint solution.

import gdb

_mode = "kernel"

_kernel_symbol_file = "Kernel.elf"
_userland_symbol_file = None
_userland_text_base = None
_userland_data_base = None
_userland_bss_base = None

class KernelCommand(gdb.Command):
    def __init__(self):
        super().__init__(
            "kernel",
            gdb.COMMAND_USER)

    def invoke(self, argument, from_tty):
        _mode = "kernel"

        gdb.execute("symbol-file")
        gdb.execute(f"file {_kernel_symbol_file}")

class UserlandCommand(gdb.Command):
    def __init__(self):
        super().__init__(
            "userland",
            gdb.COMMAND_USER)

    def invoke(self, argument, from_tty):
        _mode = "userland"

        gdb.execute("symbol-file")

        if _userland_symbol_file:
            gdb.execute(f"add-symbol-file {_userland_symbol_file} -s .text {_userland_text_base} -s .data {_userland_data_base} -s .bss {_userland_bss_base}")

class DynamicLoaderBreakpoint(gdb.Breakpoint):
    _function_symbol = "inform_debugger_about_executable"
    _variable_symbol = "executable_for_debugger"

    def __init__(self):
        super().__init__(
            spec=self._function_symbol,
            type=gdb.BP_BREAKPOINT,
            internal=True,
            temporary=False,
            qualified=False)

    def stop(self):
        executable = gdb.parse_and_eval(self._variable_symbol)

        _userland_symbol_file = "Userland/Shell.1.elf"

        _userland_text_base = hex(executable["m_text_base"])
        _userland_data_base = hex(executable["m_data_base"])
        _userland_bss_base = hex(executable["m_bss_base"])

        if _mode == "userland":
            gdb.execute("userland")

        return False

KernelCommand()
UserlandCommand()

gdb.execute("kernel")

DynamicLoaderBreakpoint()
