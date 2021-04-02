#pragma once

#include <iostream>
#include <functional>
#include <vector>

namespace Tests
{
    using TestFunction = std::function<void()>;

    struct TestCase;

    std::vector<TestCase*>& tests();

    struct TestCase
    {
        TestCase(const char *name, const char *file, size_t line, TestFunction func)
            : m_name(name)
            , m_file(file)
            , m_line(line)
            , m_func(func)
        {
            tests().push_back(this);
        }

        const char *m_name;
        const char *m_file;
        size_t m_line;
        TestFunction m_func;
    };

    inline std::vector<TestCase*>& tests()
    {
        static std::vector<TestCase*> tests;
        return tests;
    }

    void run()
    {
        for (auto *test : tests())
        {
            std::cout << "Running test '" << test->m_name << "' (" << test->m_file << ":" << test->m_line << ")\n";
            std::cout.flush();

            test->m_func();
        }
    }
}

#define TEST_CASE(name) \
    void __test_func_##name(); \
    ::Tests::TestCase __test_case_##name { #name, __FILE__, __LINE__, __test_func_##name }; \
    void __test_func_##name()

#define TEST_MAIN() \
    int main(int argc, char **argv) { ::Tests::run(); }
