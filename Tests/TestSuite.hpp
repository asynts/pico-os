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
#include <Std/Format.hpp>

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

    inline void run()
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

    struct Tracker
    {
        static constexpr bool debug = false;

        static size_t m_create_count;
        static size_t m_move_count;
        static size_t m_copy_count;
        static size_t m_destroy_count;

        static void clear()
        {
            m_create_count = 0;
            m_move_count = 0;
            m_copy_count = 0;
            m_destroy_count = 0;
        }

        static void assert(
            std::optional<size_t> create_count,
            std::optional<size_t> move_count,
            std::optional<size_t> copy_count,
            std::optional<size_t> destroy_count,
            std::string_view filename = __builtin_FILE(),
            size_t line = __builtin_LINE())
        {
            if (debug) {
                std::cout << "current: ";
                dump();

                std::cout << "asserting: create=";
                if (create_count.has_value())
                    std::cout << *create_count;
                else
                    std::cout << "?";

                std::cout << ", move=";
                if (move_count.has_value())
                    std::cout << *move_count;
                else
                    std::cout << "?";

                std::cout << ", copy=";
                if (copy_count.has_value())
                    std::cout << *copy_count;
                else
                    std::cout << "?";

                std::cout << ", destroy=";
                if (destroy_count.has_value())
                    std::cout << *destroy_count;
                else
                    std::cout << "?";
                std::cout << "\n" << filename << ":" << line << "\n";
            }

            if (create_count)
                ASSERT(m_create_count == *create_count);
            if (move_count)
                ASSERT(m_move_count == *move_count);
            if (copy_count)
                ASSERT(m_copy_count == *copy_count);
            if (destroy_count)
                ASSERT(m_destroy_count == *destroy_count);
        }

        static void dump()
        {
            std::cout << "create=" << m_create_count << ", move=" << m_move_count << ", copy=" << m_copy_count << ", destroy=" << m_destroy_count << '\n';
        }

        Tracker()
            : m_value(0)
        {
            ++m_create_count;
            if (debug)
                std::cout << "Tracker()\n";
        }
        Tracker(int value)
            : m_value(value)
        {
            ++m_create_count;
            if (debug)
                std::cout << "Tracker(" << value << ")\n";
        }
        Tracker(const Tracker& other)
        {
            m_value = other.m_value;
            ++m_copy_count;
            if (debug)
                std::cout << "Tracker(const Tracker&)\n";
        }
        Tracker(Tracker&& other)
        {
            m_value = exchange(other.m_value, 0);
            ++m_move_count;
            if (debug)
                std::cout << "Tracker(Tracker&&)\n";
        }
        ~Tracker()
        {
            ++m_destroy_count;
            if (debug)
                std::cout << "~Tracker()\n";
        }

        Tracker& operator=(const Tracker& other)
        {
            ++m_copy_count;
            m_value = other.m_value;
            return *this;
        }
        Tracker& operator=(Tracker&& other)
        {
            ++m_move_count;
            m_value = exchange(other.m_value, 0);
            return *this;
        }

        auto operator<=>(const Tracker& other) const = default;

        int m_value;
    };
}

template<>
struct Std::Formatter<Tests::Tracker> {
    static void format(Std::StringBuilder& builder, const Tests::Tracker& value)
    {
        builder.appendf("{}", value.m_value);
    }
};

#define TEST_CASE(name) \
    void __test_func_##name(); \
    ::Tests::TestCase __test_case_##name { #name, __FILE__, __LINE__, __test_func_##name }; \
    void __test_func_##name()

#define TEST_MAIN() \
    int main(int argc, char **argv) { ::Tests::run(); }
