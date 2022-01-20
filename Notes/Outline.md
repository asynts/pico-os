### Open Questions

Some things that need to be established before I can start with the implementation:

-   I need to have a good understanding of filesystems.
    Last time I didn't quite understand it when I started.

-   How can I unit test parts of the system?

-   Are modules strictly hierachical?
    Is it possible for `foo.bar.baz` to include `foo.another`?

-   How can I test/run the system on actual hardware?
    Last time this was quite laborious.
    Maybe I can setup QEMU for development?

-   I also need to write some sort of linker script.

### System Calls

-   `sys$read`
-   `sys$write`
-   `sys$open`
-   `sys$close`
-   `sys$fstat`
-   `sys$wait`
-   `sys$exit`
-   `sys$chdir`
-   `sys$getcwd`
-   `sys$posix_spawn`

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
-   `BootLoader`
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
