#pragma once

#include <Std/Vector.hpp>
#include <Std/Optional.hpp>

namespace Std {
    template<typename T, typename S>
    class Map {
    public:
        template<typename T_, typename S_>
        void append(T_&& source, S_&& target)
        {
            m_sources.append(forward<T_>(source));
            m_targets.append(forward<S_>(target));
        }

        Optional<S> lookup(T source)
        {
            for (usize index = 0; index < m_sources.size(); ++index)
            {
                if (m_sources[index] == source)
                    return m_targets[index];
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
