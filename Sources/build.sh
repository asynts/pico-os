source Sources/workaround/build.sh
source Sources/kernel/build.sh
source Sources/kit/build.sh
source Sources/boot/build.sh

function step_build() {
    step_build_workaround
    step_build_kit
    step_build_kernel
    step_build_boot
}
