export module nested.bar.baz;

import nested.foo;

namespace nested
{
    void bar() {
        foo();
    }
}
