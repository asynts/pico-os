#include <Std/Forward.hpp>
#include <Kernel/Interface/syscalls.h>
#include <pico/printf.h>
#include <pico/stdio.h>

#define STDIN_FILENO 0

class TypeErasedArgument {
public:
    i32 fd() const { return *reinterpret_cast<const i32*>(&m_value); }
    usize size() const { return m_value; }

    template<typename T>
    T* pointer() { return reinterpret_cast<T*>(m_value); }

private:
    u32 m_value;
};

extern "C"
isize isr_svcall(u32 syscall, TypeErasedArgument arg1, TypeErasedArgument arg2, TypeErasedArgument arg3)
{
    if (syscall == _SC_readline) {
        int fd = arg1.fd();
        char *buffer = arg2.pointer<char>();
        usize *buffer_size = arg3.pointer<usize>();

        printf("syscall: readline(%i, %p, %zu)\n", fd, buffer, *buffer_size);

        assert(fd == STDIN_FILENO);

        // FIXME: Read commands from console.
        const char *command = "echo Hello, world!\n";

        if (*buffer_size < __builtin_strlen(command) + 1) {
            *buffer_size = __builtin_strlen(command) + 1;

            return -ERANGE;
        }

        *buffer_size = __builtin_strlen(command) + 1;
        __builtin_memcpy(buffer, command, *buffer_size);

        return -ESUCCESS;
    }

    if (syscall == _SC_dmesg) {
        char *message = arg1.pointer<char>();

        printf("syscall: dmesg(%p)\n", message);

        printf("dmesg: %s\n", message);

        return -ESUCCESS;
    }

    panic("Unknown system call %i", syscall);
}
