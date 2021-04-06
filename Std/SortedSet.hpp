#pragma once

#include <Std/Forward.hpp>
#include <Std/StringBuilder.hpp>

namespace Std
{
    // FIXME: Upgrade to red/black trees

    // FIXME: Add iterator capabilities

    template<typename T>
    class SortedSet {
    public:
        SortedSet()
        {
            m_root = nullptr;
            m_size = 0;
        }
        ~SortedSet()
        {
            delete m_root;
        }

        T* search(const T& value)
        {
            Node *parent = nullptr;
            Node *node = search_impl(value, &parent, m_root);

            if (node != nullptr)
                return &node->m_value;
            else
                return nullptr;
        }

        void dump(StringBuilder& builder) const
        {
            if (m_root)
                m_root->dump(builder);
            else
                builder.append("nil");
        }

        T& insert(const T& value)
        {
            return insert_impl(value);
        }
        T& insert(T&& value)
        {
            return insert_impl(move(value));
        }

        const T* min() const
        {
            return const_cast<SortedSet*>(this)->min();
        }
        T* min()
        {
            Node *parent = nullptr;
            Node *node = min_impl(m_root, &parent);

            if (node)
                return &node->m_value;
            else
                return nullptr;
        }

        void remove(const T& value)
        {
            Node *parent = nullptr;
            Node *node = search_impl(value, &parent, m_root);

            if (node == nullptr)
                return;

            if (node->m_left == nullptr || node->m_right == nullptr)
                return remove_impl(node, parent);

            Node *min_in_right_parent = node;
            Node *min_in_right = min_impl(node->m_right, &min_in_right_parent);
            ASSERT(min_in_right);

            node->m_value = move(min_in_right->m_value);

            remove_impl(min_in_right, min_in_right_parent);
        }

        usize size() const { return m_size; }

    private:
        struct Node {
            Node(const T& value)
                : m_value(value)
            {
                m_left = nullptr;
                m_right = nullptr;
            }
            Node(T&& value)
                : m_value(move(value))
            {
                m_left = nullptr;
                m_right = nullptr;
            }

            ~Node()
            {
                delete m_left;
                delete m_right;
            }

            void dump(StringBuilder& builder) const
            {
                if (m_left == nullptr && m_right == nullptr) {
                    builder.appendf("%", m_value);
                    return;
                }

                builder.append('(');

                if (m_left)
                    m_left->dump(builder);
                else
                    builder.append("nil");

                builder.appendf(" % ", m_value);

                if (m_right)
                    m_right->dump(builder);
                else
                    builder.append("nil");

                builder.append(')');
            }

            void replace_child(Node *old, Node *new_)
            {
                if (m_left == old)
                    m_left = new_;
                else if (m_right == old)
                    m_right = new_;
                else
                    ASSERT_NOT_REACHED();
            }

            T m_value;
            Node *m_left;
            Node *m_right;
        };

        template<typename T_>
        T& insert_impl(T_&& value)
        {
            Node *parent = nullptr;
            Node *node = search_impl(value, &parent, m_root);

            if (node) {
                node->m_value = forward<T_>(value);

                return node->m_value;
            } else if (parent == nullptr) {
                node = m_root = new Node { forward<T_>(value) };
                ++m_size;

                return node->m_value;
            } else if (value < parent->m_value) {
                ASSERT(parent->m_left == nullptr);
                node = parent->m_left = new Node { forward<T_>(value) };
                ++m_size;

                return node->m_value;
            } else {
                ASSERT(value > parent->m_value);

                ASSERT(parent->m_right == nullptr);
                node = parent->m_right = new Node { forward<T_>(value) };
                ++m_size;

                return node->m_value;
            }
        }

        Node* search_impl(const T& value, Node **parent, Node *subtree)
        {
            if (subtree == nullptr)
                return nullptr;

            if (value < subtree->m_value) {
                *parent = subtree;
                return search_impl(value, parent, subtree->m_left);
            } else if (value > subtree->m_value) {
                *parent = subtree;
                return search_impl(value, parent, subtree->m_right);
            } else {
                return subtree;
            }
        }

        Node* min_impl(Node* subtree, Node **parent)
        {
            if (subtree == nullptr)
                return nullptr;

            if (subtree->m_left == nullptr)
                return subtree;

            *parent = subtree;
            return min_impl(subtree->m_left, parent);
        }

        void remove_impl(Node *node, Node *parent)
        {
            ASSERT(node->m_left == nullptr || node->m_right == nullptr);

            if (node->m_left == nullptr && node->m_right == nullptr) {
                if (parent)
                    parent->replace_child(node, nullptr);
                else
                    m_root = nullptr;
            } else if (node->m_left == nullptr) {
                if (parent)
                    parent->replace_child(node, node->m_right);
                else
                    m_root = node->m_right;

                node->m_right = nullptr;
            } else {
                if (parent)
                    parent->replace_child(node, node->m_left);
                else
                    m_root = node->m_left;

                node->m_left = nullptr;
            }

            delete node;
            --m_size;
        }

        Node *m_root;
        usize m_size;
    };

    template<typename T>
    struct Formatter<SortedSet<T>> {
        static void format(StringBuilder& builder, const SortedSet<T>& value)
        {
            return value.dump(builder);
        }
    };
}
