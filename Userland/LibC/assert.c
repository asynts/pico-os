#include <assert.h>
#include <sys/system.h>

void breakpoint(const char *filename, size_t line)
{
    // The filename and line can be obtained from the backtrace, provided,
    // that the compiler did not optimize them away
    volatile const char *filename_ = filename;
    volatile size_t line_ = line;

    const char *message = "USERLAND ASSERTION FAILED\n";
    sys$write(STDOUT_FILENO, message, __builtin_strlen(message));

    asm volatile("bkpt #0");
}
