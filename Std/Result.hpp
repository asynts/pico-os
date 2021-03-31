#pragma once

namespace Std
{
    template<typename ValueType, typename ErrorType>
    class Result {
    public:
        Result(const ValueType& value)
            : m_value(value)
            , m_ok(true)
        {
        }
        Result(ValueType&& value)
            : m_value(move(value))
            , m_ok(true)
        {
        }

        Result(const ErrorType& error)
            : m_error(error)
            , m_ok(false)
        {
        }
        Result(ErrorType&& error)
            : m_error(move(error))
            , m_ok(false)
        {
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
