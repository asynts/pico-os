#pragma once

#if !defined(USERLAND) && !defined(KERNEL) && !defined(HOST)
# error "USERLAND, KERNEL or HOST needs to be defined"
#endif

#if defined(KERNEL) || defined(HOST)
# include <Std/Types.hpp>
#endif

#if defined(USERLAND)
#define S_IFMT  (0b1111 << 0)
#define S_IFDIR (0b0001 << 0)
#define S_IFREG (0b0010 << 0)
#define S_IFDEV (0b0011 << 0)

#define S_IRWXU (0b1111 << 4)
#define S_IRUSR (0b0001 << 4)
#define S_IWUSR (0b0010 << 4)
#define S_IXUSR (0b0100 << 4)

#define S_IRWXG (0b1111 << 8)
#define S_IRGRP (0b0001 << 8)
#define S_IWGRP (0b0010 << 8)
#define S_IXGRP (0b0100 << 8)

#define S_IRWXO (0b1111 << 12)
#define S_IROTH (0b0001 << 12)
#define S_IWOTH (0b0010 << 12)
#define S_IXOTH (0b0100 << 12)
#elif defined(KERNEL) || defined(HOST)
namespace Kernel
{
    static_assert(sizeof(unsigned int) == 4);
    enum class ModeFlags : unsigned int {
        Format    = 0b1111 << 0,
        Directory = 0b0001 << 0,
        Device    = 0b0010 << 0,
        Regular   = 0b0011 << 0,

        UserPermissions   = 0b1111 << 4,
        UserReadAccess    = 0b0001 << 4,
        UserWriteAccess   = 0b0010 << 4,
        UserExecuteAccess = 0b0100 << 4,

        GroupPermissions   = 0b1111 << 8,
        GroupReadAccess    = 0b0001 << 8,
        GroupWriteAccess   = 0b0010 << 8,
        GroupExecuteAccess = 0b0100 << 8,

        OthersPermissions   = 0b1111 << 12,
        OthersReadAccess    = 0b0001 << 12,
        OthersWriteAccess   = 0b0010 << 12,
        OthersExecuteAccess = 0b0100 << 12,

        Invalid = 0,
        DefaultPermissions = UserReadAccess | UserWriteAccess | GroupReadAccess | GroupWriteAccess | OthersReadAccess,
        DefaultExecutablePermissions = DefaultPermissions | UserExecuteAccess | GroupExecuteAccess | OthersExecuteAccess,
    };

    inline ModeFlags operator|(ModeFlags lhs, ModeFlags rhs)
    {
        return static_cast<ModeFlags>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
    }
    inline ModeFlags operator&(ModeFlags lhs, ModeFlags rhs)
    {
        return static_cast<ModeFlags>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
    }

    enum class FileSystemId : u32 {
        Invalid,
        Flash,
        Ram,
    };
}
#endif

#if defined(USERLAND)
typedef unsigned int dev_t;
typedef unsigned int ino_t;
typedef unsigned int mode_t;
typedef int off_t;
typedef unsigned int blksize_t;
typedef unsigned int blkcnt_t;
typedef unsigned int uid_t;
typedef unsigned int gid_t;

struct stat {
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    dev_t st_rdev;
    off_t st_size;
    blksize_t st_blksize;
    blkcnt_t st_blocks;
    uid_t st_uid;
    gid_t st_gid;
};

struct dirent {
    char d_name[252];
    ino_t d_ino;
};
#endif

#if defined(KERNEL)
namespace Kernel
{
    struct UserlandFileInfo {
        FileSystemId st_dev;
        u32 st_ino;
        ModeFlags st_mode;
        u32 st_rdev;
        i32 st_size;
        u32 st_blksize;
        u32 st_blocks;
        u32 st_uid;
        u32 st_gid;
    };
    struct FileInfo {
        // FIXME: This is ugly
        FileSystemId st_dev;
        u32 st_ino;
        ModeFlags st_mode;
        u32 st_rdev;
        i32 st_size;
        u32 st_blksize;
        u32 st_blocks;
        u32 st_uid;
        u32 st_gid;

        u8 *m_data;
    };

    struct UserlandDirectoryInfo {
        char d_name[252];
        u32 d_ino;
    };
    struct FlashDirectoryInfo {
        char m_name[252];
        FileInfo *m_info;
    };
}
#endif

#if defined(HOST)
namespace Kernel
{
    struct FileInfo {
        // FIXME: This is ugly
        FileSystemId st_dev;
        u32 st_ino;
        ModeFlags st_mode;
        u32 st_rdev;
        i32 st_size;
        u32 st_blksize;
        u32 st_blocks;
        u32 st_uid;
        u32 st_gid;

        u32 m_data;
    };

    struct FlashDirectoryInfo {
        char m_name[252];
        u32 m_info_raw;
    };
}
#endif
