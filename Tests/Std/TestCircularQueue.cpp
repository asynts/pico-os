#include <Tests/TestSuite.hpp>

#include <Std/CircularQueue.hpp>

TEST_CASE(circularqueue)
{
    Std::CircularQueue<int, 4> queue;

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);

    ASSERT(queue.size() == 3);

    ASSERT(queue.dequeue() == 1);
    ASSERT(queue.dequeue() == 2);
    ASSERT(queue.dequeue() == 3);

    ASSERT(queue.size() == 0);
}

TEST_CASE(circularqueue_destroy)
{
    Tests::Tracker::clear();

    {
        Std::CircularQueue<Tests::Tracker, 4> queue;

        Tests::Tracker::assert(0, 0, 0, 0);

        queue.enqueue({});

        Tests::Tracker::assert(1, 1, 0, 1);

        queue.enqueue({});

        Tests::Tracker::assert(2, 2, 0, 2);

        queue.dequeue();

        Tests::Tracker::assert(2, 3, 0, 4);
    }

    Tests::Tracker::assert(2, 3, 0, 5);
}

TEST_CASE(circularqueue_wrap)
{
    Std::CircularQueue<int, 3> queue;

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);

    queue.dequeue();

    queue.enqueue(4);

    ASSERT(queue.size() == 3);
    ASSERT(queue.dequeue() == 2);
    ASSERT(queue.dequeue() == 3);
    ASSERT(queue.dequeue() == 4);
}

TEST_MAIN();
