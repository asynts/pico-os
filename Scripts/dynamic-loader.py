import gdb

class DynamicLoaderBreakpoint(gdb.Breakpoint):
    _symbol_name = "dynamic_load_debugger_hook"

    def __init__(self):
        super().__init__(
            spec=self._symbol_name,
            type=gdb.BP_HARDWARE_WATCHPOINT ,
            wp_class=gdb.WP_WRITE,
            internal=False,
            temporary=False,
            qualified=False)

    def stop(self):
        print("HIT DYNAMIC LOADER BREAKPOINT!")
        loaded_executable = gdb.parse_and_eval(self._symbol_name)
        print(loaded_executable)

DynamicLoaderBreakpoint()
