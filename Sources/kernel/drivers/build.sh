function step_build_kernel_drivers() {
    [[ -d Build/kernel/drivers ]] || mkdir -p Build/kernel/drivers

    compile_cxx "kernel/drivers/module.cpp" keep
}
