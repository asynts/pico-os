#pragma once

#include <Std/String.hpp>
#include <Std/Optional.hpp>
#include <Std/RefPtr.hpp>

#include <Kernel/Forward.hpp>
#include <Kernel/PageAllocator.hpp>
#include <Kernel/SystemHandler.hpp>
#include <Kernel/MPU.hpp>
#include <Kernel/StackWrapper.hpp>
#include <Kernel/Interface/Types.hpp>
#include <Kernel/Process.hpp>

namespace Kernel
{
    constexpr bool debug_thread = false;

    class Thread : public RefCounted<Thread> {
    public:
        ImmutableString m_name;
        volatile bool m_privileged = false;

        // There are two situations where this could be true:
        //
        //  1. We want the last reference to this thread to be dropped.
        //     The scheduler is holding the last reference in it's queue.
        //
        //  2. The thread blocked and something else is keeping the reference.
        volatile bool m_masked_from_scheduler = false;

        volatile bool m_is_default_thread = false;

        Optional<FullRegisterContext*> m_stashed_context;
        RefPtr<Process> m_process;

        Vector<MPU::Region> m_regions;
        Vector<OwnedPageRange> m_owned_page_ranges;

        virtual ~Thread()
        {
            if (debug_thread)
                dbgln("[Thread::~Thread] m_name='{}'", m_name);
        }

        template<typename Callback>
        void setup_context(Callback&& callback)
        {
            auto& stack = m_owned_page_ranges.append(PageAllocator::the().allocate(PageAllocator::stack_power).must());

            auto& stack_region = m_regions.append({});
            stack_region.rbar.region = 0;
            stack_region.rbar.valid = 0;
            stack_region.rbar.addr = u32(stack.data()) >> 5;
            stack_region.rasr.enable = 1;
            stack_region.rasr.size = MPU::compute_size(stack.size());
            stack_region.rasr.srd = 0b00000000;
            stack_region.rasr.attrs_b = 1;
            stack_region.rasr.attrs_c = 1;
            stack_region.rasr.attrs_s = 1;
            stack_region.rasr.attrs_tex = 0b000;
            stack_region.rasr.attrs_ap = 0b011;
            stack_region.rasr.attrs_xn = 1;

            StackWrapper stack_wrapper { stack.bytes() };

            auto callback_container = [this, callback_ = move(callback)]() mutable {
                callback_();

                die();
            };
            using CallbackContainer = decltype(callback_container);

            u8 *callback_container_on_stack = stack_wrapper.reserve(sizeof(CallbackContainer));

            // FIXME: We hardfault right here, it seems that 'callback_container_on_stack' makes no sense
            new (callback_container_on_stack) CallbackContainer { move(callback_container) };

            void (*callback_container_wrapper)(void*) = type_erased_member_function_wrapper<CallbackContainer, &CallbackContainer::operator()>;

            setup_context_impl(stack_wrapper, callback_container_wrapper, callback_container_on_stack);
        }

        void stash_context(FullRegisterContext& context)
        {
            VERIFY(!m_stashed_context.is_valid());
            m_stashed_context = &context;
        }

        FullRegisterContext& unstash_context()
        {
            auto& context = *m_stashed_context.must();
            m_stashed_context.clear();

            return context;
        }

        void set_masked_from_scheduler(bool masked);
        void wakeup();

        i32 syscall(u32 syscall, TypeErasedValue, TypeErasedValue, TypeErasedValue);

        i32 sys$read(i32 fd, u8 *buffer, usize count);
        i32 sys$write(i32 fd, const u8 *buffer, usize count);
        i32 sys$open(const char *pathname, u32 flags, u32 mode);
        i32 sys$close(i32 fd);
        i32 sys$fstat(i32 fd, UserlandFileInfo *statbuf);
        i32 sys$wait(i32 *status);
        i32 sys$exit(i32 status);
        i32 sys$chdir(const char *pathname);
        i32 sys$get_working_directory(u8 *buffer, usize *size);

        i32 sys$posix_spawn(
            i32 *pid,
            const char *pathname,
            const UserlandSpawnFileActions *file_actions,
            const UserlandSpawnAttributes *attrp,
            char **argv,
            char **envp);

    private:
        friend RefCounted<Thread>;
        explicit Thread(ImmutableString name);

        void setup_context_impl(StackWrapper, void (*callback)(void*), void* argument);
        void die();
    };
}
