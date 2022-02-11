#include <Kit/Singleton.hpp>
#include <Kit/Assertions.hpp>

// FIXME: Copy the unit test library over.

using namespace Kit;

struct A : Singleton<A> {
private:
    friend Singleton<A>;
    A() = default;
};

struct B : Singleton<B> {
private:
    friend Singleton<B>;
    B() = default;
};

int main() {
    A::initialize();
    B::initialize();

    ASSERT(&A::the() == &A::the());
    ASSERT(reinterpret_cast<void*>(&B::the()) != reinterpret_cast<void*>(&A::the()));

    return 0;
}
