picosdk_inc = include_directories(
    '../../Extern/include',
    '../../Extern/pico-sdk/src/rp2040/hardware_structs/include',
    '../../Extern/pico-sdk/src/rp2040/hardware_regs/include',
    '../../Extern/pico-sdk/src/rp2_common/hardware_base/include',
    '../../Extern/pico-sdk/src/rp2_common/pico_platform/include',
    '../../Extern/pico-sdk/src/common/pico_base/include',
    is_system: true,
)

kernel_lib = static_library('Kernel',
    'Drivers/UartOutputDriver.cpp',
    include_directories : [
        '..',
        picosdk_inc,
    ])
