#include <Std/Forward.hpp>

namespace Std {
    [[noreturn]]
    void crash(const char *filename, usize line);

    void assert_that(bool condition, const char *filename, usize line);
}

#define ASSERT(...) ::Std::assert_that(__VA_ARGS__, __FILE__, __LINE__)
#define ASSERT_NOT_REACHED() ::Std::crash(__FILE__, __LINE__)
#define TODO() ::Std::crash(__FILE__, __LINE__)

#define CRASH() ::Std::crash(__FILE__, __LINE__)
