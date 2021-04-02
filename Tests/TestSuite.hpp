#pragma once

// Otherwise IntelliSense will be incorrect.
#ifndef TEST
# warning "TEST not defined"
# define TEST
#endif

#include <iostream>
#include <functional>
#include <vector>
#include <span>

#include <Std/Span.hpp>

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

    template<typename T, usize Size>
    void dump_span(std::span<T, Size> span)
    {
        std::cout << "{ ";

        auto prefix = "";
        for (T& value : span) {
            std::cout << prefix;

            if (std::is_same_v<std::remove_const_t<T>, char>)
                std::cout << "'" << value << "'";
            else
                std::cout << value;

            prefix = ", ";
        }

        std::cout << " }\n";
    }

    template<typename T>
    void dump_span(Std::Span<T> span)
    {
        return dump_span(std::span { span.data(), span.size() });
    }
}

#define TEST_CASE(name) \
    void __test_func_##name(); \
    ::Tests::TestCase __test_case_##name { #name, __FILE__, __LINE__, __test_func_##name }; \
    void __test_func_##name()

#define TEST_MAIN() \
    int main(int argc, char **argv) { ::Tests::run(); }
