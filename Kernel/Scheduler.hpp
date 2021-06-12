#pragma once

#include <Std/Singleton.hpp>
#include <Std/String.hpp>
#include <Std/CircularQueue.hpp>
#include <Std/OwnPtr.hpp>
#include <Std/ArmedScopeGuard.hpp>

#include <Kernel/Process.hpp>
#include <Kernel/SystemHandler.hpp>
#include <Kernel/PageAllocator.hpp>
#include <Kernel/Thread.hpp>
#include <Kernel/StackWrapper.hpp>

namespace Kernel
{
    using namespace Std;

    class SchedulerGuard;

    class Scheduler : public Singleton<Scheduler> {
    public:
        template<typename T, void (T::*Method)()>
        static void wrap_member_function_call_magic(void *object)
        {
            (reinterpret_cast<T*>(object)->*Method)();
        }

        Thread& active_thread()
        {
            return m_threads.front();
        }

        void schedule_next_without_saving_context();

        void terminate_active_thread();
        void donate_my_remaining_cpu_slice();

        Thread& create_thread(Thread&& thread)
        {
            VERIFY(thread.m_context.is_valid());
            return m_threads.enqueue(move(thread));
        }

        template<typename Callback>
        Thread& create_thread(Thread&& thread, Callback&& callback)
        {
            auto wrapper = [callback_ = move(callback)]() mutable {
                dbgln("Calling wrapper in create_thread");

                callback_();
                FIXME();
            };

            VERIFY(thread.m_owned_ranges.size() == 0);

            auto stack_range = PageAllocator::the().allocate(power_of_two(0x800)).must();
            StackWrapper stack { { reinterpret_cast<u8*>(stack_range.m_base), stack_range.size() } };
            thread.m_owned_ranges.append(stack_range);

            dbgln("[Scheduler::create_thread] Allocated stack for thread '{}'", thread.m_name);

            u8 *moved_wrapper = stack.reserve(sizeof(decltype(wrapper)));
            new (moved_wrapper) decltype(wrapper) { move(wrapper) };

            void (*wrapper_wrapper_function_pointer)(void*) = wrap_member_function_call_magic<decltype(wrapper), &decltype(wrapper)::operator()>;

            return create_thread_impl(move(thread), stack, wrapper_wrapper_function_pointer, moved_wrapper);
        }

        template<typename Callback>
        Thread& create_thread(StringView name, Callback&& callback)
        {
            Thread thread { String::format("Kernel: {}", name) };

            return create_thread(move(thread), move(callback));
        }

        [[noreturn]]
        void loop();

        // Used by PendSV exception handler should return Thread.m_stack.m_current_stack
        FullRegisterContext* schedule_next(FullRegisterContext*);

        bool enabled() const { return m_enabled; }

    private:
        friend Singleton<Scheduler>;
        Scheduler();

        Thread& create_thread_impl(Thread&& thread, StackWrapper, void (*callback)(void*), void *this_);

        CircularQueue<Thread, 8> m_threads;

        friend SchedulerGuard;
        volatile bool m_enabled;
    };

    class SchedulerGuard {
    public:
        SchedulerGuard()
        {
            m_armed = exchange(Scheduler::the().m_enabled, false);
        }
        ~SchedulerGuard()
        {
            if (m_armed)
                Scheduler::the().m_enabled = true;
        }
        SchedulerGuard(SchedulerGuard&& other)
        {
            m_armed = exchange(other.m_armed, false);
        }

        SchedulerGuard(const SchedulerGuard&) = delete;
        SchedulerGuard& operator=(const SchedulerGuard&) = delete;
        SchedulerGuard& operator=(SchedulerGuard&&) = delete;

    private:
        bool m_armed;
    };
}
