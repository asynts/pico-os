source Sources/kernel/drivers/build.sh

function step_build_kernel() {
    [[ -d Build/kernel ]] || mkdir -p Build/kernel

    step_build_kernel_drivers

    compile_cxx "kernel/module.cpp" keep
}
