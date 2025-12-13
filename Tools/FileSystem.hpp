#pragma once

#include <filesystem>
#include <map>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <LibElf/Generator.hpp>
#include <LibElf/RelocationTable.hpp>
#include <LibElf/SymbolTable.hpp>

#include <Kernel/Interface/Types.hpp>

class FileSystem {
public:
  explicit FileSystem(Elf::Generator &generator);
  ~FileSystem();

  // Add a file from memory buffer to the filesystem
  uint32_t
  add_file(Elf::MemoryStream &stream,
           Kernel::ModeFlags mode = Kernel::ModeFlags::Regular |
                                    Kernel::ModeFlags::DefaultPermissions,
           uint32_t inode_number = 0, size_t *load_offset = nullptr);

  // Add a file from the host filesystem
  uint32_t
  add_host_file(const std::filesystem::path &path,
                Kernel::ModeFlags mode = Kernel::ModeFlags::Regular |
                                         Kernel::ModeFlags::DefaultPermissions);

  // Add a directory containing the specified files
  uint32_t add_directory(std::map<std::string, uint32_t> &files,
                         uint32_t inode_number = 0);

  // Add the root directory
  uint32_t add_root_directory(std::map<std::string, uint32_t> &files);

  // Finalize the filesystem and write section to the ELF generator
  void finalize();

private:
  Elf::Generator &m_generator;
  bool m_finalized = false;

  std::optional<Elf::RelocationTable> m_data_relocs;
  std::optional<size_t> m_data_index;
  std::optional<size_t> m_base_symbol;

  Elf::MemoryStream m_data_stream;

  std::map<uint32_t, uint32_t> m_inode_to_offset;

  size_t m_next_inode = 3;
};
