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
    if (syscall == _SC_read) {
        i32 fd = arg1.fd();
        char *buffer = arg2.pointer<char>();
        usize count = arg3.size();

        // dbgprintf("syscall: read(%i, %p, %zu)\n", fd, buffer, count);

        assert(fd == STDIN_FILENO);

        for (usize index = 0; index < count; ++index)
            buffer[index] = uart_getc(uart0);

        return count;
    }

    if (syscall == _SC_write) {
        i32 fd = arg1.fd();
        const char *buffer = arg2.pointer<const char>();
        usize count = arg3.size();

        // dbgprintf("syscall: write(%i, %p, %zu)\n", fd, buffer, count);

        assert(fd == STDOUT_FILENO);

        uart_write_blocking(uart0, (uint8_t*)buffer, count);

        return count;
    }

    panic("Unknown system call %i", syscall);
}
