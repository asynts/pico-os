#pragma once

#include <Std/Forward.hpp>
#include <Std/StringBuilder.hpp>

namespace Std {
// A Red-Black Tree implementation of a sorted set.
// Invariants:
// 1. Every node is either red or black.
// 2. The root is always black.
// 3. Every leaf (NIL) is black.
// 4. If a node is red, then both its children are black.
// 5. For each node, all simple paths from the node to descendant leaves contain
// the same number of black nodes.
template <typename T> class SortedSet {
public:
  enum class Color { Red, Black };

  struct Node {
    T m_value;
    Color m_color;
    Node *m_left;
    Node *m_right;
    Node *m_parent;

    Node(const T &value)
        : m_value(value), m_color(Color::Red), m_left(nullptr),
          m_right(nullptr), m_parent(nullptr) {}
    Node(T &&value)
        : m_value(move(value)), m_color(Color::Red), m_left(nullptr),
          m_right(nullptr), m_parent(nullptr) {}

    ~Node() {
      delete m_left;
      delete m_right;
    }

    void dump(StringBuilder &builder) const {
      if (!m_left && !m_right) {
        builder.appendf("{}{}", m_value, m_color == Color::Red ? "(R)" : "(B)");
        return;
      }
      builder.append('(');
      if (m_left)
        m_left->dump(builder);
      else
        builder.append("nil");
      builder.appendf(" {}{} ", m_value, m_color == Color::Red ? "(R)" : "(B)");
      if (m_right)
        m_right->dump(builder);
      else
        builder.append("nil");
      builder.append(')');
    }
  };

  SortedSet() : m_root(nullptr), m_size(0) {}

  ~SortedSet() { clear(); }

  SortedSet(const SortedSet &) = delete;
  SortedSet &operator=(const SortedSet &) = delete;

  SortedSet(SortedSet &&other) : m_root(nullptr), m_size(0) {
    *this = move(other);
  }

  SortedSet &operator=(SortedSet &&other) {
    clear();
    m_root = exchange(other.m_root, nullptr);
    m_size = exchange(other.m_size, 0);
    return *this;
  }

  void clear() {
    delete m_root;
    m_root = nullptr;
    m_size = 0;
  }

  usize size() const { return m_size; }

  class InorderIterator {
  public:
    InorderIterator(SortedSet &set, Node *root) : m_set(set), m_current(root) {}

    InorderIterator begin() { return *this; }
    InorderIterator end() { return {m_set, nullptr}; }
    bool is_end() { return *this == end(); }

    const T &operator*() const { return m_current->m_value; }
    T &operator*() { return m_current->m_value; }

    InorderIterator &operator++() {
      if (!m_current)
        return *this;
      if (m_current->m_right) {
        m_current = m_current->m_right;
        while (m_current->m_left)
          m_current = m_current->m_left;
        return *this;
      }
      Node *p = m_current->m_parent;
      while (p && m_current == p->m_right) {
        m_current = p;
        p = p->m_parent;
      }
      m_current = p;
      return *this;
    }

    InorderIterator operator++(int) {
      InorderIterator copy = *this;
      ++(*this);
      return copy;
    }

    bool operator==(const InorderIterator &other) const {
      return m_current == other.m_current;
    }
    bool operator!=(const InorderIterator &other) const {
      return !(*this == other);
    }

  private:
    SortedSet &m_set;
    Node *m_current;
  };

  InorderIterator inorder() {
    Node *min = m_root;
    while (min && min->m_left)
      min = min->m_left;
    return InorderIterator{*this, min};
  }

  // Searches for a value in the set.
  // Returns a pointer to the value if found, or nullptr otherwise.
  // Complexity: O(log n)
  T *search(const T &value) {
    Node *current = m_root;
    while (current) {
      if (value < current->m_value)
        current = current->m_left;
      else if (current->m_value < value)
        current = current->m_right;
      else
        return &current->m_value;
    }
    return nullptr;
  }

  // Inserts a value into the set.
  // If the value already exists, the existing node is effectively essentially
  // "overwritten" or kept (set semantics typically imply unique keys, but this
  // implementation updates the value if found in the loop before this helper).
  // Wait, the current implementation *returns* existing if found?
  // Ah, `insert_impl` replaces if found.
  // Complexity: O(log n)
  T &insert(const T &value) { return insert_impl(value); }
  T &insert(T &&value) { return insert_impl(move(value)); }

  void remove(const T &value) {
    Node *z = m_root;
    while (z) {
      // If value < z->m_value, go left.
      // If z->m_value < value, go right.
      // Else match.
      if (value < z->m_value)
        z = z->m_left;
      else if (z->m_value < value)
        z = z->m_right;
      else
        break;
    }
    if (!z)
      return;

    Node *y = z;
    Node *x;
    Node *x_parent;
    Color y_original_color = y->m_color;

    if (!z->m_left) {
      x = z->m_right;
      x_parent = z->m_parent;
      transplant(z, z->m_right);
      z->m_right = nullptr;
    } else if (!z->m_right) {
      x = z->m_left;
      x_parent = z->m_parent;
      transplant(z, z->m_left);
      z->m_left = nullptr;
    } else {
      y = z->m_right;
      while (y->m_left)
        y = y->m_left;
      y_original_color = y->m_color;
      x = y->m_right;

      if (y->m_parent == z) {
        x_parent = y;
      } else {
        x_parent = y->m_parent;
        transplant(y, y->m_right);
        y->m_right = z->m_right;
        if (y->m_right)
          y->m_right->m_parent = y;
      }
      transplant(z, y);
      y->m_left = z->m_left;
      y->m_left->m_parent = y;
      y->m_color = z->m_color;

      z->m_left = nullptr;
      z->m_right = nullptr;
    }

    z->m_left = nullptr;
    z->m_right = nullptr;
    delete z;
    m_size--;

    if (y_original_color == Color::Black)
      delete_fixup(x, x_parent);
  }

  void dump(StringBuilder &builder) const {
    if (m_root)
      m_root->dump(builder);
    else
      builder.append("nil");
  }

private:
  Node *m_root;
  usize m_size;

  template <typename T_> T &insert_impl(T_ &&value) {
    Node *z = new Node(forward<T_>(value));
    Node *y = nullptr;
    Node *x = m_root;

    while (x) {
      y = x;
      if (z->m_value < x->m_value)
        x = x->m_left;
      else if (x->m_value < z->m_value)
        x = x->m_right;
      else {
        x->m_value = forward<T_>(value);
        delete z;
        return x->m_value;
      }
    }

    z->m_parent = y;
    if (!y)
      m_root = z;
    else if (z->m_value < y->m_value)
      y->m_left = z;
    else
      y->m_right = z;

    m_size++;
    insert_fixup(z);
    return z->m_value;
  }

  void left_rotate(Node *x) {
    Node *y = x->m_right;
    x->m_right = y->m_left;
    if (y->m_left)
      y->m_left->m_parent = x;
    y->m_parent = x->m_parent;
    if (!x->m_parent)
      m_root = y;
    else if (x == x->m_parent->m_left)
      x->m_parent->m_left = y;
    else
      x->m_parent->m_right = y;
    y->m_left = x;
    x->m_parent = y;
  }

  void right_rotate(Node *y) {
    Node *x = y->m_left;
    y->m_left = x->m_right;
    if (x->m_right)
      x->m_right->m_parent = y;
    x->m_parent = y->m_parent;
    if (!y->m_parent)
      m_root = x;
    else if (y == y->m_parent->m_right)
      y->m_parent->m_right = x;
    else
      y->m_parent->m_left = x;
    x->m_right = y;
    y->m_parent = x;
  }

  void insert_fixup(Node *z) {
    while (z->m_parent && z->m_parent->m_color == Color::Red) {
      if (z->m_parent == z->m_parent->m_parent->m_left) {
        Node *y = z->m_parent->m_parent->m_right;
        if (y && y->m_color == Color::Red) {
          z->m_parent->m_color = Color::Black;
          y->m_color = Color::Black;
          z->m_parent->m_parent->m_color = Color::Red;
          z = z->m_parent->m_parent;
        } else {
          if (z == z->m_parent->m_right) {
            z = z->m_parent;
            left_rotate(z);
          }
          z->m_parent->m_color = Color::Black;
          z->m_parent->m_parent->m_color = Color::Red;
          right_rotate(z->m_parent->m_parent);
        }
      } else {
        Node *y = z->m_parent->m_parent->m_left;
        if (y && y->m_color == Color::Red) {
          z->m_parent->m_color = Color::Black;
          y->m_color = Color::Black;
          z->m_parent->m_parent->m_color = Color::Red;
          z = z->m_parent->m_parent;
        } else {
          if (z == z->m_parent->m_left) {
            z = z->m_parent;
            right_rotate(z);
          }
          z->m_parent->m_color = Color::Black;
          z->m_parent->m_parent->m_color = Color::Red;
          left_rotate(z->m_parent->m_parent);
        }
      }
    }
    m_root->m_color = Color::Black;
  }

  void transplant(Node *u, Node *v) {
    if (!u->m_parent)
      m_root = v;
    else if (u == u->m_parent->m_left)
      u->m_parent->m_left = v;
    else
      u->m_parent->m_right = v;
    if (v)
      v->m_parent = u->m_parent;
  }

  void delete_fixup(Node *x, Node *parent) {
    while (x != m_root && (!x || x->m_color == Color::Black)) {
      if (x == parent->m_left) {
        Node *w = parent->m_right;
        // w cannot be null if x is null (black height)
        if (w->m_color == Color::Red) {
          w->m_color = Color::Black;
          parent->m_color = Color::Red;
          left_rotate(parent);
          w = parent->m_right;
        }
        if ((!w->m_left || w->m_left->m_color == Color::Black) &&
            (!w->m_right || w->m_right->m_color == Color::Black)) {
          w->m_color = Color::Red;
          x = parent;
          parent = x->m_parent;
        } else {
          if (!w->m_right || w->m_right->m_color == Color::Black) {
            if (w->m_left)
              w->m_left->m_color = Color::Black;
            w->m_color = Color::Red;
            right_rotate(w);
            w = parent->m_right;
          }
          w->m_color = parent->m_color;
          parent->m_color = Color::Black;
          if (w->m_right)
            w->m_right->m_color = Color::Black;
          left_rotate(parent);
          x = m_root;
          // parent doesn't matter, loop terminates
        }
      } else {
        Node *w = parent->m_left;
        if (w->m_color == Color::Red) {
          w->m_color = Color::Black;
          parent->m_color = Color::Red;
          right_rotate(parent);
          w = parent->m_left;
        }
        if ((!w->m_right || w->m_right->m_color == Color::Black) &&
            (!w->m_left || w->m_left->m_color == Color::Black)) {
          w->m_color = Color::Red;
          x = parent;
          parent = x->m_parent;
        } else {
          if (!w->m_left || w->m_left->m_color == Color::Black) {
            if (w->m_right)
              w->m_right->m_color = Color::Black;
            w->m_color = Color::Red;
            left_rotate(w);
            w = parent->m_left;
          }
          w->m_color = parent->m_color;
          parent->m_color = Color::Black;
          if (w->m_left)
            w->m_left->m_color = Color::Black;
          right_rotate(parent);
          x = m_root;
        }
      }
    }
    if (x)
      x->m_color = Color::Black;
  }
};

template <typename T> struct Formatter<SortedSet<T>> {
  static void format(StringBuilder &builder, const SortedSet<T> &value) {
    value.dump(builder);
  }
};
} // namespace Std
