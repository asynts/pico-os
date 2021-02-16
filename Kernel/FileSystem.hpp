#pragma once

// FIXME: I forgot about indices :)

// FIXME: There are a ton of things completely wrong here.

namespace Kernel {
    enum class FileType : u8 {
        RegularFile,
        Directory,
    };

    enum class FileIndex : u32 {
    };

    struct FileInfo {
        Type type;
        usize size;
    };
    static_assert(sizeof(FileInfo) == 8);

    [[gnu::packed]]
    struct FileNode {
        FileInfo info;

        Array<u8*, 20> direct_block_pointers;
        Array<u8**, 10> indirect_block_pointers;
    };
    static_assert(sizeof(FileNode) == 128);

    [[gnu::packed]]
    struct DirectoryNode {
        FileInfo info;

        Array<Array<PackedString<128>, 4>*, 20> direct_entry_pointers;
        Array<Array<PackedString<128>, 4>**, 10> indirect_entry_pointers;
    };
    static_assert(sizeof(DirectoryNode) == 128);

    class FileSystem : public Singleton<FileSystem> {
    public:
        FileSystem();

        Result<FileInfo> create_file(Path);
        Result<FileInfo> create_directory(Path);

        Result<Void> remove(Path);
    };
}
