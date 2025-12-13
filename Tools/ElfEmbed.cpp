#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include <fmt/format.h>

#include <LibElf/Generator.hpp>
#include <LibElf/MemoryStream.hpp>
#include <LibElf/SymbolTable.hpp>

#include "FileSystem.hpp"

// Writes the generated ELF stream to the specified output path
static void write_output_file(const std::filesystem::path &path,
                              Elf::MemoryStream &stream) {
  fmt::print("Writing output file {}\n", path.string());

  std::ofstream outfile(path,
                        std::ios::binary | std::ios::out | std::ios::trunc);
  if (!outfile.is_open()) {
    fmt::print(stderr, "Error: Could not open {} for writing.\n",
               path.string());
    exit(1);
  }

  // Create a buffer to hold the data
  stream.copy_to_stream(outfile);
  outfile.close();
}

int main(int argc, char **argv) {
  if (argc < 3) {
    fmt::print(stderr,
               "Usage: {} <output_file> <input_file1> [input_file2 ...]\n",
               argv[0]);
    return 1;
  }

  std::filesystem::path output_path = argv[1];
  std::vector<std::filesystem::path> input_files;

  for (int i = 2; i < argc; ++i) {
    input_files.emplace_back(argv[i]);
  }

  Elf::Generator generator;
  FileSystem fs{generator};

  std::map<std::string, uint32_t> bin_files;

  for (const auto &file_path : input_files) {
    if (!std::filesystem::exists(file_path)) {
      fmt::print(stderr, "Error: Input file {} does not exist.\n",
                 file_path.string());
      return 1;
    }

    std::string filename = file_path.filename().string();
    fmt::print("Embedding {}\n", filename);

    // Add file to filesystem (executable permissions by default for these
    // tools)
    bin_files[filename] = fs.add_host_file(
        file_path, Kernel::ModeFlags::Regular |
                       Kernel::ModeFlags::DefaultExecutablePermissions);
  }

  fs.add_root_directory(bin_files);
  fs.finalize();

  auto stream = generator.finalize();
  write_output_file(output_path, stream);

  return 0;
}
