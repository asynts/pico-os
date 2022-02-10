function step_build_std_integers() {
    [[ -d Build/std/integers ]] || mkdir -p Build/std/integers

    compile_cxx "std/integers/module.cpp" keep
}
