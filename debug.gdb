# TODO Get the 'tasks.dbg' command working again

file Kernel.elf
target extended-remote localhost:3333

define dis_here
    x/20i ($pc -20)
end

define si_and_dis
    si
    dis_here
end

define rebuild
    shell ninja
    load
    monitor reset init
end

set confirm off

set history save on
set history size unlimited
set history remove-duplicates 1
