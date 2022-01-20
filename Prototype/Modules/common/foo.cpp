module common:foo;

// Notice that we are able to import another partition here.
import common:baz;

int foo() {
    return 42;
}

// This specializes a template defined in 'common:baz'.
template<>
int baz<double>() {
    return 2;
}
