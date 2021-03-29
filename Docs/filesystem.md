### Classes

#### FileInfo

~~~c++
struct FileInfo {
    u32 m_ino;
    u32 m_filesystem;
    u32 m_flags;
    u32 m_size;
    u32 m_device;

    u32 m_data;
};
~~~

This is called `inode` in Linux, it is very similar to the disk representation. There is exactly one
object for each file, it is shared if the same file is referenced multiple times.

There are a few common properties, but the meaning of `FileInfo::m_data` is defined by each filesystem.

#### DirectoryEntryInfo

~~~c++
struct DirectoryEntryInfo {
    FileInfo *m_info;
    Optional<Map<String, DirectoryEntryInfo*>> m_entries;
};
~~~

This is called `dentry` in Linux, it is used to cache the directory structure for fast access. In praxis
it is not possible to store the entire representation of the tree in memory, thus the `m_entries` are lazily
loaded.

#### FileHandle

~~~c++
struct FileHandle {
    DirectoryEntryInfo *m_info;
    u32 m_offset;
};
~~~

This is the kernel's view of file descriptors, each process can reference `FileHandle`s with file descriptors.

We keep a reference to the `DirectoryEntryInfo` instead of `FileInfo` because we can, this will make it impossible
to have implement some things later on, like temporary files that are not listed in the filesystem, but this
should simplify a great deal of things.

#### Process

~~~c++
struct Process {
    Map<u32, FileHandle*> m_file_descriptors;
    u32 m_next_file_descriptor;
};
~~~

Processes can reference file handles with file descriptors, which simply start at zero.
