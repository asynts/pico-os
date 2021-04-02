#include <Tests/TestSuite.hpp>

namespace Tests
{
    size_t Tracker::m_create_count = 0;
    size_t Tracker::m_copy_count = 0;
    size_t Tracker::m_move_count = 0;
    size_t Tracker::m_destroy_count = 0;
}
