# Minimum Viable Product

- Filesystem
  - Create files.
  - Read from files.
  - Write to files.
  - Navigate directories.

- Programs
  - Stored in the file system as ELF binary.
    - Position Indepentent Code
    - Has the following segments: .text, .data, .bss
  - Can be loaded by the kernel.
  - Define `int main(int argc, char **argv)` function.
  - Executed in unprivileged mode.

- Shell
  - `echo` command
    - `puts` function
      - `write` syscall
  - `cd` command
    - `chdir` syscall
  - `cat`
    - `open` syscall
    - `read` syscall
    - `close` syscall
  - `ls` command
    - `opendir` function
      - `open` syscall (O_DIRECTORY)
    - `readdir` function
      - `read` syscall
    - `closedir` function
      - `close` syscall
