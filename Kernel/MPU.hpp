#pragma once

#include <hardware/structs/mpu.h>

namespace Kernel::MPU
{
    // FIXME: This API sucks

    union RASR {
        struct {
            u32 enable : 1;
            u32 size : 5;
            u32 reserved_4 : 2;
            u32 srd : 8;
            u32 attrs_b : 1;
            u32 attrs_c : 1;
            u32 attrs_s : 1;
            u32 attrs_tex : 3;
            u32 reserved_3 : 2;
            u32 attrs_ap : 3;
            u32 reserved_2 : 1;
            u32 attrs_xn : 1;
            u32 reserved_1 : 3;
        };
        u32 raw;
    };
    static_assert(sizeof(RASR) == 4);

    union CTRL {
        struct {
            u32 enable : 1;
            u32 hfnmiena : 1;
            u32 privdefena : 1;
            u32 reserved_1 : 29;
        };
        u32 raw;
    };
    static_assert(sizeof(CTRL) == 4);

    union RBAR {
        struct {
            u32 region : 4;
            u32 valid : 1;
            u32 addr : 27;
        };
        u32 raw;
    };
    static_assert(sizeof(RBAR) == 4);

    inline CTRL ctrl()
    {
        return static_cast<CTRL>(mpu_hw->ctrl);
    }
    inline void set_ctrl(CTRL ctrl)
    {
        mpu_hw->ctrl = ctrl.raw;
    }

    inline RASR rasr()
    {
        return static_cast<RASR>(mpu_hw->rasr);
    }
    inline void set_rasr(RASR rasr)
    {
        mpu_hw->rasr = rasr.raw;
    }

    inline RBAR rbar()
    {
        return static_cast<RBAR>(mpu_hw->rbar);
    }
    inline void set_rbar(RBAR rbar)
    {
        mpu_hw->rbar = rbar.raw;
    }

    inline usize compute_size(usize size)
    {
        VERIFY(__builtin_popcount(size) == 1);

        usize power_of_two = __builtin_ctzl(size);

        VERIFY(power_of_two >= 0);

        return power_of_two - 1;
    }

    inline void dump()
    {
        dbgln("[MPU::dump]");
        dbgln("  CTRL={} RNR={} TYPE={}", ctrl().raw, u32(mpu_hw->rnr), u32(mpu_hw->type));

        u32 rnr = mpu_hw->rnr;

        for (size_t index = 0; index < 8; ++index) {
            mpu_hw->rnr = index;
            dbgln("  RBAR={} RASR={}", rbar().raw, rasr().raw);
        }

        mpu_hw->rnr = rnr;
    }
}
