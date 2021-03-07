import gdb

class DynamicLoaderBreakpoint(gdb.Breakpoint):
    _symbol_name = "inform_debugger_about_executable"

    def __init__(self):
        super().__init__(
            spec=self._symbol_name,
            type=gdb.BP_BREAKPOINT,
            internal=True,
            temporary=False,
            qualified=False)

    def stop(self):
        print("HIT DYNAMIC LOADER BREAKPOINT!")
        loaded_executable = gdb.parse_and_eval() # FIXME
        print(loaded_executable)

DynamicLoaderBreakpoint()
