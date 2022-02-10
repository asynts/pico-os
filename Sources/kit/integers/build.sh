function step_build_kit_integers() {
    [[ -d Build/kit/integers ]] || mkdir -p Build/kit/integers

    compile_cxx "kit/integers/module.cpp" keep
}
