#pragma once

#include <Std/Singleton.hpp>
#include <Std/Span.hpp>

#include <Kernel/FileSystem/VirtualFileSystem.hpp>

namespace Kernel {
class ConsoleFileHandle final : public VirtualFileHandle {
public:
  explicit ConsoleFileHandle(VirtualFile &file) : m_file(file) {}

  KernelResult<usize> read(Bytes bytes) override;
  KernelResult<usize> write(ReadonlyBytes bytes) override;

  VirtualFile &file() override;

private:
  VirtualFile &m_file;
};

class ConsoleFile final : public Singleton<ConsoleFile>, public VirtualFile {
public:
  VirtualFileHandle &create_handle_impl() override {
    return *new ConsoleFileHandle(*this);
  }

  void truncate() override { VERIFY_NOT_REACHED(); }

private:
  friend Singleton<ConsoleFile>;
  ConsoleFile();
};
} // namespace Kernel
