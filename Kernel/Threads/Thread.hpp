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

    private:
        void setup_context_impl(StackWrapper, void (*callback)(void*), void* argument);
    };
}
