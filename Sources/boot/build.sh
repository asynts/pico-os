function step_build_boot() {
    [[ -d Build/boot ]] || mkdir -p Build/boot

    compile_asm "boot/boot_1_debugger.S" keep

    # First, we compile the assembly file.
    # Then, we insert the checksum with a custom python script.
    compile_asm "boot/boot_2_flash.S" discard
    python3 Scripts/checksum.py Build/boot/boot_2_flash.S.o Build/boot/boot_2_flash.patched.o
    OBJS+=("Build/boot/boot_2_flash.patched.o")

    compile_asm "boot/boot_3_vectors.S" keep

    compile_cxx "boot/boot_4_load_kernel.cpp" keep
}
