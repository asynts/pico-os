#pragma once

#define S_IRUSR 0400
#define S_IWUSR 0200
#define S_IRGRP 0040
#define S_IWGRP 0020
#define S_IROTH 0004

// FIXME: These types are already defined in the kernel, why?
#ifdef USERLAND
typedef unsigned int dev_t;
typedef unsigned int ino_t;
typedef unsigned int mode_t;
typedef int off_t;
typedef unsigned int blksize_t;
typedef unsigned int blkcnt_t;
#endif

#ifdef USERLAND
struct stat {
#elif defined(KERNEL)
struct UserlandFileInfo {
#else
#error "Neither USERLAND nor KERNEL defined."
#endif
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    dev_t st_rdev;
    off_t st_size;
    blksize_t st_blksize;
    blkcnt_t st_blocks;
};

#ifdef USERLAND
struct dirent {
#elif defined(KERNEL)
struct UserlandDirectoryInfo {
#else
#error "Neither USERLAND nor KERNEL defined."
#endif
    char d_name[256];
};
