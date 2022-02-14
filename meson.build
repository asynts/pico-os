project('pico-os', 'cpp',
    default_options : [ 'b_staticpic=false' ])

subdir('Sources/Kit/Tests')
subdir('Sources/Boot')

# FIXME: This has to depend on 'link.ld' changes.
executable('System.elf',
    link_with : [ boot_lib ],
    link_args : [ '-static', '-T', meson.source_root() / 'Sources/link.ld' ])
