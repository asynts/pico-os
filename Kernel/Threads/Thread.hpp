#pragma once

#include <Std/String.hpp>
#include <Std/Optional.hpp>

#include <Kernel/Forward.hpp>
#include <Kernel/PageAllocator.hpp>
#include <Kernel/SystemHandler.hpp>
#include <Kernel/MPU.hpp>
#include <Kernel/StackWrapper.hpp>

namespace Kernel
{
    class Thread {
    public:
        String m_name;
        bool m_privileged = false;
        bool m_die_at_next_opportunity = false;
        bool m_blocked = false;

        Optional<FullRegisterContext*> m_stashed_context;
        Optional<Process*> m_process;

        // FIXME: I don't like this so much
        Optional<SystemCallInfo> m_running_system_call;

        Vector<MPU::Region> m_regions;
        Vector<OwnedPageRange> m_owned_page_ranges;

        explicit Thread(String name)
            : m_name(move(name))
        {
        }

        template<typename Callback>
        void setup_context(Callback&& callback)
        {
            auto& stack = m_owned_page_ranges.append(PageAllocator::the().allocate_owned(PageAllocator::stack_power).must());

            StackWrapper stack_wrapper { stack.bytes() };

            auto callback_container = [callback_ = move(callback)]() mutable {
                callback_();
                FIXME();
            };
            using CallbackContainer = decltype(callback_container);

            u8 *callback_container_on_stack = stack_wrapper.reserve(sizeof(CallbackContainer));
            new (callback_container_on_stack) CallbackContainer { move(callback_container) };

            void (*callback_container_wrapper)(void*) = type_erased_member_function_wrapper<CallbackContainer, &CallbackContainer::operator()>;

            setup_context_impl(stack_wrapper, callback_container_wrapper, callback_container_on_stack);
        }

        void stash_context(FullRegisterContext& context)
        {
            dbgln("[Thread::stash_context] m_name='{}' this={}", m_name, this);
            dbgln("{}", context);

            VERIFY(!m_stashed_context.is_valid());
            m_stashed_context = &context;
        }

        FullRegisterContext& unstash_context()
        {
            dbgln("[Thread::unstash_context] m_name='{}' this={}", m_name, this);

            auto& context = *m_stashed_context.must();
            m_stashed_context.clear();

            dbgln("{}", context);

            return context;
        }

    private:
        void setup_context_impl(StackWrapper, void (*callback)(void*), void* argument);
    };
}
