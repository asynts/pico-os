### Features

  - Add buddy page-allocator instead of allocating twice as much memory for alignment.

### Tweaks

  - `Editor.elf` can now loads the file passed as first argument

### Bugs:

  - `argv` and `envp` were allocated in kernel memory and thus inaccessible to userland processes
