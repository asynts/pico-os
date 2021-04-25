#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include <Kernel/Interface/System.hpp>

int errno;

// Remember to update the kernel aswell.
const char *const strerror_lookup[] = {
    "Success",
    [ENOTDIR] = "Not a directory",
    [EINTR] = "Interrupted system call",
    [ERANGE] = "Numerical result out of range",
    [ENOENT] = "No such file or directory",
    [EACCES] = "Permission denied",
    [EISDIR] = "Is a directory",
};

uint32_t _pc_base();

char* strerror(int error)
{
    assert(error >= 0 && error < EMAX);

    // FIXME: The compiler "forgets" to add the program counter when computing the
    //        address.  I was not able to reproduce this with a smaller program, it
    //        appears, the environment influences this hick-up
    const char *error_pointer_thingy = strerror_lookup[error] + _pc_base();

    return (char*)strdup(error_pointer_thingy);
}
