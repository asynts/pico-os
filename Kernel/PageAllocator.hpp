#pragma once

#include <Std/Singleton.hpp>
#include <Std/Array.hpp>
#include <Std/Optional.hpp>
#include <Std/Format.hpp>

#include <Kernel/Forward.hpp>

namespace Kernel
{
    inline bool debug_page_allocator = false;

    struct PageRange {
        usize m_power;
        uptr m_base;

        const u8* data() const { return reinterpret_cast<const u8*>(m_base); }
        u8* data() { return reinterpret_cast<u8*>(m_base); }

        usize size() const { return 1 << m_power; }

        ReadonlyBytes bytes() const { return { data(), size() }; }
        Bytes bytes() { return { data(), size() }; }
    };

    class OwnedPageRange {
    public:
        explicit OwnedPageRange(PageRange range)
            : m_range(range)
        {
        }
        OwnedPageRange(const OwnedPageRange&) = delete;
        OwnedPageRange(OwnedPageRange&& other)
        {
            m_range = move(other.m_range);
        }
        ~OwnedPageRange();

        OwnedPageRange& operator=(const OwnedPageRange&) = delete;

        OwnedPageRange& operator=(OwnedPageRange&& other)
        {
            m_range = move(other.m_range);
            return *this;
        }

        usize size() const { return m_range->size(); }

        const u8* data() const { return m_range->data(); }
        u8* data() { return m_range->data(); }

        ReadonlyBytes bytes() const { return m_range->bytes(); }
        Bytes bytes() { return m_range->bytes(); }

        Optional<PageRange> m_range;
    };

    class PageAllocator : public Singleton<PageAllocator> {
    public:
        static constexpr usize max_power = 18;
        static constexpr usize stack_power = power_of_two(0x800);

        Optional<OwnedPageRange> allocate(usize power);
        void deallocate(OwnedPageRange&);

        // FIXME: Syncronize
        void dump()
        {
            dbgln("[PageAllocator] blocks:");
            for (usize power = 0; power < max_power; ++power) {
                dbgln("  [{}]: {}", power, m_blocks[power]);
            }
        }

        void set_mutex_enabled(bool enabled);

    private:
        friend Singleton<PageAllocator>;
        PageAllocator();

        Optional<PageRange> allocate_locked(usize power);
        void deallocate_locked(PageRange);

        // There is quite a bit of trickery going on here:
        //
        //   - The address of this block is encoded indirectly in the address of this object
        //
        //   - The size of this block is encoded indirection in the index used to access m_blocks
        struct Block {
            Block *m_next;
        };

        Array<Block*, max_power + 1> m_blocks;
    };
}
