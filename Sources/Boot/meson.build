incdir = include_directories(
    '..',
    'include',
)

# FIXME: We need to setup the checksum somehow.
static_library('Boot',
    'boot_1_debugger.S',
    'boot_2_flash.S',
    'boot_3_vectors.S',
    'boot_4_load_kernel.cpp',
    include_directories : incdir,
    link_args : [ '-T', meson.source_root() /  'Sources/link.ld' ])
