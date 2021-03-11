#pragma once

#include <Std/Vector.hpp>
#include <Std/Optional.hpp>

namespace Std {
    template<typename T, typename S>
    class Map {
    public:
        void append(T source, S target)
        {
            m_sources.append(source);
            m_targets.append(target);
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

    private:
        Vector<T> m_sources;
        Vector<S> m_targets;
    };
}
