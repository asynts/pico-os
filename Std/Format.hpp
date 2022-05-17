#pragma once

#include <Std/Span.hpp>
#include <Std/StringView.hpp>
#include <Std/Array.hpp>
#include <Std/Vector.hpp>
#include <Std/Concepts.hpp>
#include <Std/String.hpp>
#include <Std/Lexer.hpp>
#include <Std/RefPtr.hpp>

#ifdef KERNEL
# include <Kernel/HandlerMode.hpp>
#endif

namespace Std
{
    class StringBuilder;
    class ImmutableString;

    extern volatile int dbgln_called_in_interrupt;

    template<typename T>
    struct Formatter {
        using __no_formatter_defined = void;
    };

    template<typename T, typename = void>
    struct HasFormatter {
        static constexpr bool value = true;
    };
    template<typename T>
    struct HasFormatter<T, typename Formatter<T>::__no_formatter_defined> {
        static constexpr bool value = false;
    };

    using FormatFunction = void(*)(StringBuilder&, const void*);

    struct TypeErasedFormatParameter {
        const void *m_value;
        FormatFunction m_format;
    };

    class TypeErasedFormatParams {
    public:
        const TypeErasedFormatParameter& param(usize index) const
        {
            ASSERT(index < m_params.size());
            return m_params[index];
        }

    protected:
        void set_params(Span<TypeErasedFormatParameter> params)
        {
            m_params = params;
        }

    private:
        Span<TypeErasedFormatParameter> m_params;
    };

    template<typename... Parameters>
    class VariadicFormatParams : public TypeErasedFormatParams {
    public:
        explicit VariadicFormatParams(const Parameters&... parameters)
            : m_params { TypeErasedFormatParameter {
                &parameters,
                [](StringBuilder& builder, const void *value)
                {
                    Formatter<Parameters>::format(builder, *reinterpret_cast<const Parameters*>(value));
                },
            }... }
        {
            this->set_params(m_params.span());
        }

    private:
        Array<TypeErasedFormatParameter, sizeof...(Parameters)> m_params;
    };

    void vformat(StringBuilder&, StringView fmtstr, TypeErasedFormatParams);

    class ImmutableStringInstance : public RefCounted<ImmutableStringInstance> {
    public:
        ImmutableStringInstance()
        {
            m_buffer_size = 1;
            m_buffer = new char[m_buffer_size];
            m_buffer[0] = 0;
        }
        explicit ImmutableStringInstance(StringView string)
        {
            m_buffer_size = string.size() + 1;
            m_buffer = new char[m_buffer_size];
            string.strcpy_to({ m_buffer, m_buffer_size });
        }
        ImmutableStringInstance(const ImmutableStringInstance& other)
        {
            m_buffer = other.m_buffer;
            m_buffer_size = other.m_buffer_size;
        }
        ~ImmutableStringInstance()
        {
            delete[] m_buffer;
        }

        const char* data() const { return m_buffer; }
        usize size() const {
            VERIFY(m_buffer_size >= 1);
            return m_buffer_size - 1;
        }

    private:
        char *m_buffer;
        usize m_buffer_size;
    };

    // Strings are immutable, that is very important.
    class ImmutableString {
    public:
        ImmutableString()
            : m_string_instance(ImmutableStringInstance::construct())
        {

        }

        ImmutableString(StringView string)
            : m_string_instance(ImmutableStringInstance::construct(string)) { }

        ImmutableString(const char *string)
            : m_string_instance(ImmutableStringInstance::construct(StringView{ string })) { }

        ImmutableString(const ImmutableString& other)
            : m_string_instance(other.m_string_instance) { }

        void strcpy_to(Span<char> other) const
        {
            return view().strcpy_to(other);
        }

        template<typename... Parameters>
        static ImmutableString format(StringView fmtstr, const Parameters&...);

        const char* data() const { return m_string_instance->data(); }
        usize size() const { return m_string_instance->size(); }
        const char* cstring() const { return data(); }

        Span<const char> span() const { return { data(), size() }; }
        StringView view() const { return { data(), size() }; }

        operator StringView() const { return view(); }

        std::strong_ordering operator<=>(const ImmutableString& other) const { return view() <=> other.view(); }

        bool operator==(const ImmutableString& other) const
        {
            return view() == other.view();
        }

        ImmutableString& operator=(const ImmutableString& other)
        {
            m_string_instance = other.m_string_instance;
            return *this;
        }

    private:
        RefPtr<ImmutableStringInstance> m_string_instance;
    };

    class StringBuilder {
    public:
        void append(char value)
        {
            m_data.append(value);
        }
        void append(StringView value)
        {
            for (char ch : value.iter())
                m_data.append(ch);
        }
        template<typename... Parameters>
        void appendf(StringView fmtstr, const Parameters&... parameters)
        {
            vformat(*this, fmtstr, VariadicFormatParams { parameters... });
        }

        char* data() { return m_data.data(); }
        usize size() { return m_data.size(); }

        StringView view() const { return m_data.span(); }
        ImmutableString string() const { return view(); }
        ReadonlyBytes bytes() const { return view().bytes(); }

    private:
        Vector<char, 256> m_data;
    };

    template<typename... Parameters>
    ImmutableString ImmutableString::format(StringView fmtstr, const Parameters&... parameters)
    {
        StringBuilder builder;
        builder.appendf(fmtstr, parameters...);
        return builder.string();
    }

    inline void vformat(StringBuilder& builder, StringView fmtstr, TypeErasedFormatParams params)
    {
        usize next_parameter_index = 0;
        usize curly_brace_level = 0;
        Lexer lexer { fmtstr };

        while (!lexer.eof()) {
            if (curly_brace_level == 0) {
                if (lexer.try_consume("{{")) {
                    builder.append('{');
                    continue;
                }

                if (lexer.try_consume("}}")) {
                    builder.append('}');
                    continue;
                }

                if (lexer.try_consume('{')) {
                    curly_brace_level = 1;
                    continue;
                }

                ASSERT(lexer.peek_or_null() != '}');

                builder.append(lexer.consume());
            } else {
                if (lexer.try_consume('{')) {
                    ++curly_brace_level;
                    continue;
                }

                if (lexer.try_consume('}')) {
                    --curly_brace_level;

                    if (curly_brace_level == 0) {
                        auto& parameter = params.param(next_parameter_index++);
                        parameter.m_format(builder, parameter.m_value);
                    }

                    continue;
                }

                lexer.consume();
            }
        }
    }

    void dbgln_raw(StringView);

    template<typename... Parameters>
    void dbgln(StringView fmtstr, const Parameters&... parameters)
    {
#ifdef KERNEL
        // In handler mode, we can not ensure syncronization.
        if (Kernel::is_executing_in_handler_mode()) {
            dbgln_called_in_interrupt = dbgln_called_in_interrupt + 1;
            return;
        }
#endif

        StringBuilder builder;
        vformat(builder, fmtstr, VariadicFormatParams { parameters... });
        dbgln_raw(builder.view());
    }

    template<typename... Parameters>
    void dbgln(const char *fmtstr, const Parameters&... parameters)
    {
        return dbgln(StringView { fmtstr }, parameters...);
    }

    inline void dbgln()
    {
        dbgln_raw("");
    }

    template<typename T>
    requires Concepts::Integral<T>
    struct Formatter<T> {
        static void format(StringBuilder&, T);
    };

    template<typename T>
    struct Formatter<T*> {
        static void format(StringBuilder& builder, const T *value)
        {
            if constexpr(sizeof(T*) == 4) {
                return Formatter<u32>::format(builder, reinterpret_cast<u32>(value));
            } else {
                static_assert(sizeof(T*) == 8);
                return Formatter<u64>::format(builder, reinterpret_cast<u64>(value));
            }
        }
    };
    template<typename T, usize Size>
    struct Formatter<T[Size]> : Formatter<T*> {
    };
    template<typename T>
    struct Formatter<T[]> : Formatter<T*> {
    };

    template<>
    struct Formatter<StringView> {
        static void format(StringBuilder&, StringView);
    };
    template<>
    struct Formatter<ImmutableString> : Formatter<StringView> {
    };
    template<>
    struct Formatter<const char*> : Formatter<StringView> {
    };
    template<usize Size>
    struct Formatter<char[Size]> : Formatter<StringView> {
    };

    template<>
    struct Formatter<bool> {
        static void format(StringBuilder&, bool);
    };
    template<>
    struct Formatter<char> {
        static void format(StringBuilder&, char);
    };

    template<>
    struct Formatter<Path> {
        static void format(StringBuilder&, const Path&);
    };
    template<>
    struct Formatter<StringBuilder> {
        static void format(StringBuilder& builder, const StringBuilder& value)
        {
            builder.append(value.view());
        }
    };

    template<typename T>
    struct Formatter<Span<T>> {
        static void format(StringBuilder& builder, Span<T> value)
        {
            builder.appendf("({}, {})", value.data(), value.size());
        }
    };

    template<typename T>
    struct Formatter<Optional<T>> {
        static void format(StringBuilder& builder, const Optional<T>& value)
        {
            if (value.is_valid())
                builder.appendf("{}", value.value());
            else
                builder.append("nil");
        }
    };

    template<typename T>
    struct Formatter<RefPtr<T>> : Formatter<T*> {
    };
}

using Std::dbgln;
