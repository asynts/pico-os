#pragma

#include <Std/SortedSet.hpp>

namespace Std
{
    template<typename T>
    struct Hash {
    };

    template<>
    struct Hash<u32> {
        static u32 compute(u32 value)
        {
            // https://github.com/skeeto/hash-prospector

            value ^= value >> 16;
            value *= 0x7feb352d;
            value ^= value >> 15;
            value *= 0x846ca68b;
            value ^= value >> 16;

            return value;
        }
    };

    template<typename T>
    class HashTable {
    public:
        void insert(const T& value)
        {
            m_set.insert({ Hash<T>::compute(value), value });
        }
        void insert(T&& value)
        {
            m_set.insert({ Hash<T>::compute(value), move(value) });
        }

        T* search(const T& value)
        {
            Node *node = m_set.search({ Hash<T>::compute(value), value });

            if (node)
                return &node->m_value;
            else
                return nullptr;
        }

        void remove(const T& value)
        {
            m_set.remove({ Hash<T>::compute(value), value });
        }

        usize size() const
        {
            return m_set.size();
        }

    private:
        struct Node {
            u32 m_hash;
            T m_value;

            bool operator<(const Node& other) const
            {
                if (m_hash < other.m_hash)
                    return true;

                if (m_hash > other.m_hash)
                    return false;

                return m_value < other.m_value;
            }
            bool operator>(const Node& other) const
            {
                if (m_hash > other.m_hash)
                    return true;

                if (m_hash < other.m_hash)
                    return false;

                return m_value > other.m_value;
            }
        };

        SortedSet<Node> m_set;
    };
}
