# Qubes GUI agent

- TODO: build test tools
- TODO: (watchdog) detect if the agent fails/crashes too often and disable it/return to fullscreen mode
- TODO: consider rewriting window tracking logic to use windows hooks intead of polling (I don't remember why hooks weren't used in the first place, maybe they don't work reliably for all windows since DLL injection is needed and that breaks for protected processes like winlogon)
- TODO: custom WDDM driver (maybe some time in the future)

### Environment variables

- `QUBES_INCLUDES` must contain paths containing `windows-utils`, `libvchan`, `qubesdb` and `xeniface` includes. Normally it's `<src>/qubes-windows-utils/include;<src>/qubes-core-vchan-xen/windows/include;<src>/qubes-core-qubesdb/include;<src>/qubes-vmm-xen-windows-pvdrivers/xeniface/include`.
- `QUBES_LIBS` must contain paths containing `windows-utils`, `libvchan`, `qubesdb` and `xeniface` libraries. Normally it's `<src>/qubes-windows-utils/bin;<src>/qubes-core-vchan-xen/windows/bin;<src>/qubes-core-qubesdb/windows/bin;<src>/qubes-vmm-xen-windows-pvdrivers/bin/xeniface`.

## Command-line build

`EWDK_PATH` env variable must be set to the root of MS Enterprise WDK for Windows 10/Visual Studio 2022.

`build.cmd` script builds the solution from command line using the EWDK (no need for external VS installation).

Usage: `build.cmd Release|Debug`
