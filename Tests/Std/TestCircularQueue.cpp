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

TEST_CASE(circularqueue_enqueue_front)
{
    Std::CircularQueue<int, 3> queue;

    queue.enqueue_front(1);
    queue.enqueue(2);

    ASSERT(queue.dequeue() == 1);

    queue.enqueue_front(3);
    queue.enqueue(4);

    ASSERT(queue.size() == 3);

    ASSERT(queue.dequeue() == 3);
    ASSERT(queue.dequeue() == 2);
    ASSERT(queue.dequeue() == 4);
}

TEST_CASE(circularqueue_enqueue_return_value)
{
    Std::CircularQueue<int, 8> queue;

    auto& value_1 = queue.enqueue(1);
    auto& value_2 = queue.enqueue_front(2);
    auto& value_3 = queue.enqueue(3);

    ASSERT(queue.size() == 3);
    ASSERT(queue.front() == 2);
    ASSERT(queue.back() == 3);
    ASSERT(value_1 == 1);
    ASSERT(value_2 == 2);
    ASSERT(value_3 == 3);

    value_1 = 4;

    ASSERT(queue.size() == 3);
    ASSERT(queue.front() == 2);
    ASSERT(queue.back() == 3);
    ASSERT(value_1 == 4);
    ASSERT(value_2 == 2);
    ASSERT(value_3 == 3);
}

TEST_CASE(circularqueue_move)
{
    Tests::Tracker::clear();

    {
        Std::CircularQueue<Tests::Tracker, 4> queue1;

        Tests::Tracker::assert(0, 0, 0, 0);

        queue1.enqueue({});

        ASSERT(queue1.size() == 1);

        Tests::Tracker::assert(1, 1, 0, 1);

        {
            Std::CircularQueue<Tests::Tracker, 4> queue2 = move(queue1);

            Tests::Tracker::assert(1, {}, 0, {});

            ASSERT(queue1.size() == 0);
            ASSERT(queue2.size() == 1);
        }

        Tests::Tracker::assert(1, {}, 0, {});
    }

    Tests::Tracker::assert(1, {}, 0, {});
}

TEST_CASE(circularqueue_correct_order_after_move)
{
    Std::CircularQueue<int, 4> queue1;

    queue1.enqueue(1);
    queue1.enqueue(2);

    Std::CircularQueue<int, 4> queue2 { move(queue1) };

    ASSERT(queue2.dequeue() == 1);
    ASSERT(queue2.dequeue() == 2);
}

TEST_MAIN();
