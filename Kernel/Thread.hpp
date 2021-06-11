#pragma once

#include <Std/OwnPtr.hpp>
#include <Std/Optional.hpp>

#include <Kernel/SystemHandler.hpp>

namespace Kernel
{
    class Thread {
    public:
        Thread(StringView name, Optional<NonnullOwnPtr<Process>> process = {}, Optional<FullRegisterContext*> context = {})
            : m_name(name)
            , m_process(move(process))
            , m_context(context)
        {
        }

        // FIXME: RAII
        void free_owned_ranges()
        {
            dbgln("[Thread::free_owned_ranges] name={}", m_name);

            for (auto range : m_owned_ranges.iter()) {
                PageAllocator::the().deallocate(range);
            }
            m_owned_ranges.clear();
        }

        bool m_blocked = false;
        Optional<SystemCallInfo> m_running_system_call;

        Vector<PageRange> m_owned_ranges;

        String m_name;
        Optional<NonnullOwnPtr<Process>> m_process;
        Optional<FullRegisterContext*> m_context;
        Vector<Region> m_regions;
        bool m_privileged = false;
        bool m_die_at_next_opportunity = false;
    };
}
