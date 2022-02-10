function step_build_kernel() {
    [[ -d Build/kernel ]] || mkdir -p Build/kernel

    compile_cxx "kernel/module.cpp" keep
}
