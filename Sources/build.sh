. Sources/kernel/build.sh
. Sources/kit/build.sh
. Sources/boot/build.sh

function step_build() {
    step_build_kit
    step_build_kernel
    step_build_boot
}
