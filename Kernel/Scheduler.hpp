#pragma once

#include <Std/Singleton.hpp>
#include <Std/String.hpp>
#include <Std/CircularQueue.hpp>
#include <Std/OwnPtr.hpp>
#include <Std/ArmedScopeGuard.hpp>

#include <Kernel/Process.hpp>
#include <Kernel/SystemHandler.hpp>

namespace Kernel
{
    using namespace Std;

    // FIXME: I need to figure out who should own the stack
    struct StackWrapper {
        explicit StackWrapper(Bytes bytes)
            : m_bytes(bytes)
            , m_top(bytes.data() + bytes.size())
        {
        }

        u8* reserve(usize count)
        {
            ASSERT(m_bytes.data() + count <= m_top);
            return m_top -= count;
        }

        u8* push(ReadonlyBytes bytes)
        {
            u8 *data = reserve(bytes.size());
            bytes.copy_to({ data, bytes.size() });
            return data;
        }

        template<typename T>
        T* push_value(const T& value)
        {
            u8 *data = reserve(sizeof(value));
            return new (data) T { value };
        }

        u8* align(u32 boundary)
        {
            static_assert(sizeof(u8*) == sizeof(u32));
            if (u32(m_top) % boundary != 0)
                reserve(u32(m_top) % boundary);

            return m_top;
        }

        u8* top()
        {
            return m_top;
        }

    private:
        Bytes m_bytes;
        u8 *m_top;
    };

    // FIXME: When doing the whole page allocator thing, move this into a more sensible location
    class Region {
    public:
        enum class AllowInstructionFetch : u32 {
            Yes = 0 << 28,
            No  = 1 << 28,

            Mask = 1 << 28,
        };

        enum class Access : u32 {
            ByKernelOnly   = 0b001 << 24,
            ByUserReadOnly = 0b010 << 24,
            Full           = 0b011 << 24,

            Mask = 0b111 << 24,
        };

        enum class Shareable : u32 {
            Yes = 1 << 18,
            No  = 0 << 18,

            Mask = 1 << 18,
        };

        enum class Cacheable : u32 {
            Yes = 1 << 17,
            No  = 0 << 17,

            Mask = 1 << 17,
        };

        enum class Bufferable : u32 {
            Yes = 1 << 16,
            No  = 0 << 16,

            Mask = 1 << 16,
        };

        enum class Size : u32 {
            B256 = 7 << 1,
            B512 = 8 << 1,
            K1   = 9 << 1,
            K2   = 10 << 1,
            K4   = 11 << 1,
            K8   = 12 << 1,
            K16  = 13 << 1,
            K32  = 14 << 1,
            K64  = 15 << 1,
            K128 = 16 << 1,
            K256 = 17 << 1,
            K512 = 18 << 1,
            M1   = 19 << 1,
            M2   = 20 << 1,

            Mask = 0b11111 << 1,
        };

        static Size enum_value_for_size(usize size)
        {
            VERIFY(__builtin_popcount(size) == 1);

            // FIXME: Move this into a helper function

            // FIXME: Is this calculation correct? I would expect '31 - clz' but that doesn't work
            u32 power = 30 - __builtin_clzl(size);
            VERIFY(power <= 20);

            dbgln("computed power {} (enum: {}) for size {}", power, power << 1, size);

            return static_cast<Size>(power << 1);
        }

        static usize size_for_enum_value(Size size)
        {
            VERIFY(size != Size::Mask);
            return 1 << ((static_cast<u32>(size) >> 1) + 1);
        }

        Region(
            u32 base,
            Size size,
            AllowInstructionFetch allow_instruction_fetch,
            Access access,
            Shareable shareable,
            Cacheable cacheable,
            Bufferable bufferable)
        {
            dbgln("[Region::Region] base={} size(enum)={} size(u32)={}",
                base,
                static_cast<u32>(size),
                size_for_enum_value(size));

            VERIFY(base % size_for_enum_value(size) == 0);
            m_region_base_address_register = base;

            m_region_attribute_and_size_register = u32(size)
                                                 | u32(allow_instruction_fetch)
                                                 | u32(access)
                                                 | u32(shareable)
                                                 | u32(cacheable)
                                                 | u32(bufferable);
        }

        u32 region_base_address_register() const
        {
            return m_region_base_address_register;
        }

        u32 region_attribute_and_size_register() const { return m_region_attribute_and_size_register; }

    private:
        u32 m_region_base_address_register;
        u32 m_region_attribute_and_size_register;
    };

    class Thread {
    public:
        Thread(StringView name, Optional<NonnullOwnPtr<Process>> process = {}, Optional<FullRegisterContext*> context = {})
            : m_name(name)
            , m_process(move(process))
            , m_context(context)
        {
        }

        String m_name;
        Optional<NonnullOwnPtr<Process>> m_process;
        Optional<FullRegisterContext*> m_context;
        Vector<Region> m_regions;
        bool m_privileged = false;
        bool m_die_at_next_opportunity = false;
    };

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

            usize stack_size = 0x1000;
            u8 *stack_data = new u8[stack_size];
            StackWrapper stack { { stack_data, stack_size } };

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
