#pragma once

namespace Std
{
    struct ResultValueTag {
    };
    struct ResultErrorTag {
    };

    template<typename ValueType, typename ErrorType>
    class Result {
    public:
        Result(const ValueType& value, ResultValueTag = {})
            : m_value(value)
            , m_ok(true)
        {
        }
        Result(ValueType&& value, ResultValueTag = {})
            : m_value(move(value))
            , m_ok(true)
        {
        }

        Result(const ErrorType& error, ResultErrorTag = {})
            : m_error(error)
            , m_ok(false)
        {
        }
        Result(ErrorType&& error, ResultErrorTag = {})
            : m_error(move(error))
            , m_ok(false)
        {
        }

        static Result from_value(const ValueType& value)
        {
            return Result { value, ResultValueTag{} };
        }
        static Result from_value(ValueType&& value)
        {
            return Result { move(value), ResultValueTag{} };
        }

        static Result from_error(const ErrorType& error)
        {
            return Result { error, ResultErrorTag{} };
        }
        static Result from_error(ErrorType&& error)
        {
            return Result { move(error), ResultErrorTag{} };
        }

        ~Result()
        {
            if (m_ok)
                m_value.~ValueType();
            else
                m_error.~ErrorType();
        }

        const ValueType& value() const
        {
            ASSERT(m_ok);
            return m_value;
        }
        const ErrorType& error() const
        {
            ASSERT(!m_ok);
            return m_error;
        }

        ValueType&& take_value()
        {
            ASSERT(m_ok);
            return move(m_value);
        }
        ErrorType&& take_error()
        {
            ASSERT(!m_ok);
            return move(m_error);
        }

        const ValueType& must() const
        {
            return value();
        }

        bool ok() const { return m_ok; }
        bool is_error() const { return !m_ok; }

    private:
        bool m_ok;

        union {
            ValueType m_value;
            ErrorType m_error;
        };
    };
}
