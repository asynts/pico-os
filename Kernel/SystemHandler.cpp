#include <Std/Forward.hpp>
#include <Kernel/Interface/syscalls.h>
#include <pico/printf.h>
#include <pico/stdio.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

class TypeErasedArgument {
public:
    i32 fd() const { return *reinterpret_cast<const i32*>(&m_value); }
    usize size() const { return m_value; }

    template<typename T>
    T* pointer() { return reinterpret_cast<T*>(m_value); }

private:
    u32 m_value;
};

static bool did_handout_first_line = false;

extern "C"
isize isr_svcall(u32 syscall, TypeErasedArgument arg1, TypeErasedArgument arg2, TypeErasedArgument arg3)
{
    if (syscall == _SC_readline) {
        i32 fd = arg1.fd();
        char *buffer = arg2.pointer<char>();
        usize *size = arg3.pointer<usize>();

        printf("syscall: readline(%i, %p, %zu)\n", fd, buffer, *size);

        assert(fd == STDIN_FILENO);

        if (did_handout_first_line)
            __breakpoint();
        else
            did_handout_first_line = true;

        // FIXME: Read commands from console.
        const char *command = "echo Hello, world!\n";

        if (*size < __builtin_strlen(command) + 1) {
            *size = __builtin_strlen(command) + 1;

            return -ERANGE;
        }

        *size = __builtin_strlen(command) + 1;
        __builtin_memcpy(buffer, command, *size);

        return -ESUCCESS;
    }

    if (syscall == _SC_dmesg) {
        char *message = arg1.pointer<char>();

        printf("syscall: dmesg(%p)\n", message);

        printf("dmesg: %s\n", message);

        return -ESUCCESS;
    }

    if (syscall == _SC_write) {
        i32 fd = arg1.fd();
        const char *buffer = arg2.pointer<const char>();
        usize size = arg3.size();

        assert(fd == STDOUT_FILENO);

        printf("\033[33m");
        size_t retval = fwrite(buffer, 1, size, stdout);
        assert(retval == size);
        printf("\033[0m");

        return -ESUCCESS;
    }

    panic("Unknown system call %i", syscall);
}
