export module common;

import :foo;
export import :bar;
export import :baz;
export import :dynamic;

// Notice that we have to put this after 'import :baz', otherwise, the template hasn't been declared.
template<>
int another<int>() {
    return 0;
}
