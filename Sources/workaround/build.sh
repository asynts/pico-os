function step_build_workaround() {
    [[ -d Build/workaround ]] || mkdir -p Build/workaround

    compile_cxx "workaround/module.cpp" keep
}
