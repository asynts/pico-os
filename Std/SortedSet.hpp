#pragma once

#include <Std/Forward.hpp>

namespace Std
{
    // FIXME: Upgrade to red/black trees

    // FIXME: Add iterator capabilities

    // FIXME: Add remove operation

    // FIXME: Test this thoroughly

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
            Node *parent_node;
            Node *node = search_impl(value, &parent_node, m_root);

            if (node != nullptr)
                return &node->m_value;
            else
                return nullptr;
        }

        template<typename T_ = T>
        T& insert(T&& value)
        {
            Node *parent_node;
            Node *node = search_impl(value, &parent_node, m_root);

            if (node) {
                node->m_value = forward<T_>(value);

                return node->m_value;
            } else if (parent_node == nullptr) {
                node = m_root = new Node;
                ++m_size;

                node->m_value = forward<T_>(value);
                node->m_parent = nullptr;
                node->m_left = nullptr;
                node->m_right = nullptr;

                return node->m_value;
            } else if (value < parent_node->m_value) {
                // FIXME: We sometimes hit this assertion
                ASSERT(parent_node->m_left == nullptr);
                node = parent_node->m_left = new Node;
                ++m_size;

                node->m_value = forward<T_>(value);
                node->m_parent = parent_node;
                node->m_left = nullptr;
                node->m_right = nullptr;

                return node->m_value;
            } else {
                ASSERT(value > parent_node->m_value);
                node = parent_node->m_right = new Node;
                ++m_size;

                node->m_value = forward<T_>(value);
                node->m_parent = parent_node;
                node->m_left = nullptr;
                node->m_right = nullptr;

                return node->m_value;
            }
        }

        usize size() const { return m_size; }

    private:
        struct Node {
            ~Node()
            {
                delete m_left;
                delete m_right;
            }

            T m_value;
            Node *m_parent;
            Node *m_left;
            Node *m_right;
        };

        Node* search_impl(const T& value, Node **parent_node, Node *subtree)
        {
            if (subtree == nullptr)
                return nullptr;

            if (value < subtree->m_value) {
                *parent_node = subtree;
                return search_impl(value, parent_node, subtree->m_left);
            } else if (value > subtree->m_value) {
                *parent_node = subtree;
                return search_impl(value, parent_node, subtree->m_right);
            } else {
                return subtree;
            }
        }

        Node *m_root;
        usize m_size;
    };
}
