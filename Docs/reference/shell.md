# Shell Commands

PicoOS includes a basic interactive shell (`/bin/Shell.elf`) that provides a command-line interface over UART.

## Starting the Shell

The shell starts automatically after boot. Connect via serial:
```bash
inv tty
# or: screen /dev/tty.usbmodem* 115200
```

You'll see a prompt:
```
>
```

---

## Built-in Commands

### `echo [args...]`
Print arguments to the terminal.

```
> echo Hello World
Hello World
```

### `pwd`
Print the current working directory.

```
> pwd
/
```

### `cd <path>`
Change the current working directory.

```
> cd /bin
> pwd
/bin
```

### `ls [path]`
List directory contents. If no path is given, lists the current directory.

```
> ls
Shell.elf
Example.elf
Editor.elf

> ls /dev
tty
```

### `cat <file>`
Display the contents of a file.

```
> cat /test.txt
Hello from the file!
```

### `touch <file>`
Create an empty file (or update timestamps).

```
> touch /newfile.txt
```

### `stat <path>`
Display file information.

```
> stat /dev/tty
Device: 0x00010001
Size: 0
Mode: Device
```

### `exit`
Exit the shell (terminates the process).

```
> exit
```

---

## Running External Programs

Any command that isn't a builtin is treated as an external executable:

```
> Example.elf
This output is created by '/bin/Example.elf'
We are currently executing in '/'
```

The shell searches for executables in the `PATH` environment variable (default: `/bin`).

### Passing Arguments

```
> Example.elf arg1 arg2
...
Arguments:
  'Example.elf'
  'arg1'
  'arg2'
```

---

## Special Characters

| Character | Behavior |
| :---: | :--- |
| `Backspace` (0x7F) | Delete previous character |
| `Enter` | Execute command |
| `Ctrl+C` | *Not implemented* |

---

## Limitations

- **No pipes** (`|`)
- **No redirections** (`>`, `<`)
- **No background jobs** (`&`)
- **No command history** (up/down arrows)
- **No tab completion**
- **Fixed PATH** (`/bin` only)

---

## Implementation Notes

The shell is implemented in [`Userland/Shell.c`](pico-os/Userland/Shell.c).

Key functions:
- `readline()`: Reads user input with basic line editing
- `find_executable()`: Searches PATH for programs
- `posix_spawn()`: Spawns child processes
- `wait()`: Waits for child to terminate
