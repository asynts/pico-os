. Sources/std/integers/build.sh

function step_build_std() {
    [[ -d Build/std ]] || mkdir -p Build/std

    compile_cxx "std/module.cpp" keep
}
