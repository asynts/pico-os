#pragma once

#include <Std/HashTable.hpp>
#include <Std/Optional.hpp>

namespace Std
{
    // FIXME: What about things which can't be compared at all?

    template<typename Key, typename Value>
    class HashMap {
    public:
        void set(const Key& key, const Value& value)
        {
            m_hash.insert({ key, value });
        }
        void set(Key&& key, const Value& value)
        {
            m_hash.insert({ move(key), value });
        }
        void set(const Key& key, Value&& value)
        {
            m_hash.insert({ key, move(value) });
        }
        void set(Key&& key, Value&& value)
        {
            m_hash.insert({ move(key), move(value) });
        }

        Value* get(const Key& key)
        {
            Node *node = m_hash.search({ key, {} });

            if (node)
                return &node->m_value.value();
            else
                return nullptr;
        }

        Optional<Value> get_opt(const Key& key) const
        {
            Node *node = m_hash.search({ key, {} });

            if (node)
                return node->m_value.value();
            else
                return nullptr;
        }

        void remove(const Key& key)
        {
            m_hash.remove({ key, {} });
        }

        usize size() const
        {
            return m_hash.size();
        }

        struct Node {
            Key m_key;
            Optional<Value> m_value;

            u32 hash() const { return Hash<Key>::compute(m_key); }

            bool operator<(const Node& other) const
            {
                return m_key < other.m_key;
            }
            bool operator>(const Node& other) const
            {
                return m_key > other.m_key;
            }
        };

        HashTable<Node>::Iterator iter() { return m_hash.iter(); }

    private:
        HashTable<Node> m_hash;
    };
}
