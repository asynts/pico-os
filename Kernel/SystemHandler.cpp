#include <Std/Forward.hpp>
#include <Std/Debug.hpp>
#include <Kernel/Interface/syscalls.h>

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

extern "C"
isize isr_svcall(u32 syscall, TypeErasedArgument arg1, TypeErasedArgument arg2, TypeErasedArgument arg3)
{
    if (syscall == _SC_readline) {
        i32 fd = arg1.fd();
        char *buffer = arg2.pointer<char>();
        usize *size = arg3.pointer<usize>();

        assert(fd == STDIN_FILENO);

        usize index = 0;
        while (index < *size)
        {
            char ch = buffer[index++] = uart_getc(uart0);
            uart_putc(uart0, ch);

            if (ch == '\n')
            {
                buffer[index++] = 0;
                *size = index;
                return -ESUCCESS;
            }
        }

        panic("sys$readline: line to long for buffer");
    }

    if (syscall == _SC_dmesg) {
        char *message = arg1.pointer<char>();

        // dbgprintf("syscall: dmesg(%p)\n", message);

        dbgprintf("dmesg: %s\n", message);

        return -ESUCCESS;
    }

    if (syscall == _SC_write) {
        i32 fd = arg1.fd();
        const char *buffer = arg2.pointer<const char>();
        usize size = arg3.size();

        assert(fd == STDOUT_FILENO);

        dbgprintf("\033[33m");
        uart_write_blocking(uart0, (uint8_t*)buffer, size);
        dbgprintf("\033[0m");

        return -ESUCCESS;
    }

    panic("Unknown system call %i", syscall);
}
