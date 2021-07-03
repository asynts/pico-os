#include <Tests/TestSuite.hpp>

#include <Std/Singleton.hpp>

class A : public Std::Singleton<A> {
public:
    A(const A&)
    {
        ASSERT_NOT_REACHED();
    }
    A(A&&)
    {
        ASSERT_NOT_REACHED();
    }
    ~A()
    {
        ASSERT_NOT_REACHED();
    }

private:
    friend Singleton<A>;
    A()
    {
        static bool initialized = false;

        ASSERT(!initialized);
        initialized = true;
    }
};

TEST_CASE(singleton)
{
    A::initialize();
    A::the();
}

TEST_MAIN();
