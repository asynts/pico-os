# FIXME: This is really ugly, I really want that watchpoint solution.

import gdb

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

        text_base = hex(executable["m_text_base"])
        data_base = hex(executable["m_data_base"])
        stack_base = hex(executable["m_stack_base"])

        # FIXME: This will remove Kernel.elf symbols, if we don't do this, gdb bails because the .text sections overlap.
        gdb.execute("symbol-file")

        gdb.execute(f"add-symbol-file Userland/Shell.1.elf -s .text {text_base} -s .data {data_base} -s .stack {stack_base}")

        return False

DynamicLoaderBreakpoint()
