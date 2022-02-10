function step_build_kit() {
    [[ -d Build/kit ]] || mkdir -p Build/kit

    compile_cxx "kit/module.cpp" keep
}
