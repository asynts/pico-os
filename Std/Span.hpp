#pragma once

#include <Std/Forward.hpp>

namespace Std {
    template<typename T>
    class SpanIterator;

    template<typename T>
    class Span {
    public:
        Span()
            : m_data(nullptr)
            , m_size(0)
        {
        }

        Span(T *data, usize size)
            : m_data(data)
            , m_size(size)
        {
        }

        void clear()
        {
            set_data(nullptr);
            set_size(0);
        }

        const T* data() const { return m_data; }
        T* data() { return m_data; }

        usize size() const { return m_size; }
        bool is_empty() const { return m_size == 0; }

        void set_data(T *data) { m_data = data; }
        void set_size(usize size) { m_size = size; }

        Span<const T> span() const { return *this; }
        Span<T> span() { return *this; }

        SpanIterator<const T> iter() const;
        SpanIterator<T> iter();

    private:
        T *m_data;
        usize m_size;
    };

    template<typename T>
    class SpanIterator : public Span<T> {
    public:
        SpanIterator()
            : Span<T>()
        {
        }

        SpanIterator(Span<T> span)
            : Span<T>(span)
        {
        }

        SpanIterator begin() { return *this; }
        SpanIterator end()
        {
            SpanIterator iter;
            iter.set_data(this->data() + this->size());
            iter.set_size(0);

            return iter;
        }

        const T& operator*() const { return this->data()[0]; }
        T& operator*() { return this->data()[0]; }

        SpanIterator& operator++()
        {
            this->set_data(this->data() + 1);
            this->set_size(this->size() - 1);

            return *this;
        }

        SpanIterator operator++(int)
        {
            SpanIterator copy = *this;
            operator++();
            return copy;
        }

        bool operator==(SpanIterator<T> other) const
        {
            return this->data() == other.data();
        }
    };

    template<typename T>
    SpanIterator<const T> Span<T>::iter() const { return *this; }

    template<typename T>
    SpanIterator<T> Span<T>::iter() { return *this; }

}