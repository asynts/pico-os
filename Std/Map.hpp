#pragma once

#include <Std/Vector.hpp>
#include <Std/Optional.hpp>
#include <Std/Concepts.hpp>

namespace Std
{
    template<typename T, typename S>
    class Map {
    public:
        template<typename T_ = T, typename S_ = S>
        void append(T_&& source, S_&& target)
        {
            m_sources.append(forward<T_>(source));
            m_targets.append(forward<S_>(target));
        }

        Optional<S> lookup(T source)
        {
            if (m_sources.size() == 0)
                return {};

            for (usize index = 0; index < m_sources.size(); ++index)
            {
                // There might be duplicates so we go backwards. I am suprised that I
                // can get away with this.
                if (m_sources[(m_sources.size() - 1) - index] == source)
                    return m_targets[(m_sources.size() - 1) - index];
            }

            return {};
        }

        bool is_empty() const
        {
            return m_sources.is_empty();
        }

    private:
        Vector<T> m_sources;
        Vector<S> m_targets;
    };
}
