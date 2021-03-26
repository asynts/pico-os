#pragma once

#define _SC_read 1
#define _SC_write 2
#define _SC_open 3
#define _SC_close 4

#define O_RDONLY (1 << 0)
#define O_WRONLY (2 << 0)

#define O_DIRECTORY (1 << 4)
#define O_CREAT (1 << 5)
#define O_TRUNC (1 << 6)

#define ENOTDIR 1
