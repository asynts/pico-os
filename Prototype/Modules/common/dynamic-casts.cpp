export module common:dynamic;

struct A {
    virtual ~A() = default;
};

struct B : A {
};

export bool do_dynamic_cast() {
    B b;

    A& ref = b;

    return dynamic_cast<B*>(&ref) == &b;
}
