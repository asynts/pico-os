#include <Std/Forward.hpp>

namespace Std {
    [[noreturn]]
    void crash(const char *filename, usize line);

    inline void assert_that(bool condition, const char *filename, usize line)
    {
        if (!condition)
            crash(filename, line);
    }
}

#define ASSERT(...) ::Std::assert_that(__VA_ARGS__, __FILE__, __LINE__)
#define ASSERT_NOT_REACHED() ::Std::crash(__FILE__, __LINE__)
#define TODO() ::Std::crash(__FILE__, __LINE__)

#define CRASH() ::Std::crash(__FILE__, __LINE__)
