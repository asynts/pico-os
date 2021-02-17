#pragma once

namespace Kernel {
    constexpr usize block_size = 0x200;

    enum class FileType : u8 {
        RegularFile,
        DeviceFile,
        Directory,
    };

    enum class FileIndex : usize {
    };

    [[gnu::packed]]
    struct FileInfo {
        FileIndex index;
        Type type;
        usize size;
        bool locked;

        u8 padding_[6];
    };
    static_assert(sizeof(FileInfo) == 0x10);

    [[gnu::packed]]
    struct DirectoryEntry {
        PackedString<124> filename;
        FileIndex index;
    };
    static_assert(sizeof(DirectoryEntry) == 0x80);

    [[gnu::packed]]
    struct FileNode {
        FileInfo info;

        Array<u8*, 20> direct_block_pointers;
        Array<u8**, 8> indirect_block_pointers;
    };
    static_assert(sizeof(FileNode) == 0x80);

    [[gnu::packed]]
    struct DirectoryNode {
        FileInfo info;

        Array<Array<DirectoryEntry, 4>*, 20> direct_entry_pointers;
        Array<Array<DirectoryEntry, 4>**, 8> indirect_entry_pointers;
    };
    static_assert(sizeof(DirectoryNode) == 0x80);

    class FileSystem : public Singleton<FileSystem> {
    public:
        FileSystem();

        Result<FileIndex> create_file(Path);
        Result<FileIndex> create_directory(Path);

        Result<Void> delete_file(Path);
        Result<Void> delete_directory(Path);

        Result<FileInfo> file_info_for(FileIndex);
        Result<FileInfo> file_info_for(Path);

        FileIndex root_index();
    };

    class File {
    public:
        explicit File(FileIndex);
        explicit File(Path);

        Result<usize> read(Bytes);
        Result<usize> write(ReadonlyBytes);

        Result<LockGuard> lock();
    };

    class Directory {
    public:
        explicit Directory(FileIndex);
        explicit Directory(Path);

        Iterator<DirectoryEntry> entries();

        Result<LockGuard> lock();
    }
}
