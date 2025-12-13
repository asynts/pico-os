# Emulation Status (Deferred)

As of Dec 2025, emulation for the RP2040 SoC is not currently supported in this project.

## QEMU
*   **Status**: Unsupported in mainline.
*   **Issues**: Mainline QEMU does not have an official RP2040 model. Forks exist (e.g., `raspberrypi/qemu`) but are often outdated or require complex build setups that are out of scope for this project's standard dev environment.

## Renode
*   **Status**: Technically supported but unstable in this environment.
*   **Issues**: We encountered socket binding errors (`AddressAlreadyInUse`) and reliable headless logging issues when integrating with the CI/Test scripts.

## Future Path
Once mainline QEMU adds RP2040 support (specifically dual-core M0+ and correct XIP flash emulation), we will re-enable emulation.
