project('pico-os', 'cpp')

subdir('Sources/Kit/Tests')
subdir('Sources/Boot')

# FIXME: This has to depend on 'link.ld' changes.
executable('System',
    link_with : [ boot_lib ],
    link_args : [ '-static', '-T', meson.source_root() / 'Sources/link.ld' ])
