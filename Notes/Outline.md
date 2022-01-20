### Open Questions

Some things that need to be established before I can start with the implementation:

-   Can I use `dynamic_cast` or do I need to link something for that?

-   I need to have a good understanding of filesystems.
    Last time I didn't quite understand it when I started.

-   How can I unit test parts of the system?

-   Are modules strictly hierachical?
    Is it possible for `foo.bar.baz` to include `foo.another`?

### System Calls

-   `sys$read`
-   `sys$write`
-   `sys$open`
-   `sys$close`
-   `sys$fstat`

-   `sys$wait`
    I don't think we should use this.
    In Linux there is something with the proc filesystem that makes this work.

-   `sys$exit`

-   `sys$chdir`

-   `sys$get_working_directory`
    How does Linux does this?
    I seem to recall that there are multiple alternatives.

-   `sys$posix_spawn`
    How can I avoid that process id madness.
    (Obviously, I would never spawn that many processes but with file descriptoers it is much easier to wait for a subprocess.
    The last person that uses `sys$close` destorys it truely.)

### Outline

Based on the previous implementation, here are the classes and functions that I need to implement:

#### `std`

-   `alloc.PageAllocator`
-   `alloc.MemoryAllocator`
-   `util.Singleton`
-   `util.Optional`
-   `util.Path`
-   `util.Result`
-   `strings.StringBuilder`
-   `strings.String`
-   `strings.StringView`
-   `strings.Lexer`
-   `pointers.OwnPtr`
-   `pointers.RefPtr`
-   `containers.Span`
-   `containers.Array`
-   `containers.Vector`
-   `containers.CircularQueue`
-   `containers.HashTable`
-   `containers.HashMap`

#### `kernel`

-   `GlobalPageAllocator`
-   `InterruptHandler`
-   `devices.TerminalDevice`
-   `threads.KernelMutex`
-   `threads.Scheduler`
-   `threads.Thread`

#### `kernel.filesystem`

-   `virtual.VirtualFileSystem`
-   `virtual.VirtualFile`
-   `virtual.VirtualDirectory`
-   `memory.MemoryFileSystem`
-   `memory.MemoryFile`
-   `memory.MemoryDirectory`
-   `flash.FlashFileSystem`
-   `flash.FlashFile`
-   `flash.FlashDirectory`

#### `kernel.loader`

-   `DynamicLinker`
-   `ExecutableLoader`

#### `kernel.drivers`

-   `UartDriver`
