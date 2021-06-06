### Bugs:

  - `argv` and `envp` were allocated in kernel memory and thus inaccessible to userland processes

### Tweaks

  - `Editor.elf` can now loads the file passed as first argument
