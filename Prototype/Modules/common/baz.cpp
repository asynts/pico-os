// Notice that we need to use 'export' here, otherwise, we can not use 'export' below.
// That would create the confusing error message: "error: ‘export’ may only occur after a module interface declaration".
export module common:baz;

// I tried declaring this template in 'module.cpp' and then specializing it here, but that does not appear to work this way.
export template<typename T>
int baz() {
    return 0;
}

template<>
int baz<int>() {
    return 1;
}

export template<typename T>
int another() {
    return 1;
}
