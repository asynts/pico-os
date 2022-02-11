export module workaround;

// There is a bug in gcc about exporting namespaces.
//
// If something is exported into a namespace in one module, there are sometimes issues when another module
// tries to put something into it.
//
// This bug was already reported here: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100617
//
// My own diagnosis can be found in '0018_import-recursively.md'.

export namespace kit { }
export namespace kernel { }
export namespace kernel::drivers { }
export namespace boot { }
