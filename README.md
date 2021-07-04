# Project Farm
A multiplayer farming simulator.

## Documentation
See [here](./documentation/Documentation.md) for the technical documentation.

## Build Status
### Nighty
![Nightly Windows](https://github.com/snowmeltarcade/projectfarm/actions/workflows/build_nightly_windows.yml/badge.svg)
![Nightly Mac](https://github.com/snowmeltarcade/projectfarm/actions/workflows/build_nightly_mac.yml/badge.svg)
![Nightly Linux](https://github.com/snowmeltarcade/projectfarm/actions/workflows/build_nightly_linux.yml/badge.svg)
![Nightly Linux Docker](https://github.com/snowmeltarcade/projectfarm/actions/workflows/build_nightly_linux_docker.yml/badge.svg)

### Release
![Nightly Windows](https://github.com/snowmeltarcade/projectfarm/actions/workflows/build_release_windows.yml/badge.svg)
![Nightly Mac](https://github.com/snowmeltarcade/projectfarm/actions/workflows/build_release_mac.yml/badge.svg)
![Nightly Linux](https://github.com/snowmeltarcade/projectfarm/actions/workflows/build_release_linux.yml/badge.svg)
![Nightly Linux Docker](https://github.com/snowmeltarcade/projectfarm/actions/workflows/build_release_linux_docker.yml/badge.svg)

## Build
### CLI
##### Correct as of 2021-05-20

To build, run `build.py`:
```
python3 build.py
```

`build.py` takes the following flags:
* `-i` / `--install-dependencies`
    Install the library dependencies before build. Defaults to `false`.
* `-nb` / `--no-build`
    Skip the build step. Defaults to `false`.
* `-ni` / `--no-install`
    Skip the install step. Defaults to `false`.
* `-c` / `--cleanup`
    Deletes the build and install data/temp files after build. Defaults to `false`.

After the build, the client, server and data will be copied to the `./install` directory. This directory has the following structure:

```
install
    + - _platform name_ // Windows/Darwin/Linux
        + - client
            + - _version_
                + - bin
                    + - projectfarm_client // projectfarm_client.exe on Windows
                        any other dependencies
                + - data
                    + - _data needed for runtime_
        + - latest // this will always contain the latest version of the build
            + - bin
                + - projectfarm_client // projectfarm_client.exe on Windows
                    projectfarm_server // projectfarm_server.exe on Windows
                    any other dependencies
            + - data
                + - _data needed for runtime_
        + - server
            + - _version_
                + - bin
                    + - projectfarm_server // projectfarm_server.exe on Windows
                        any other dependencies
                + - data
                    + - _data needed for runtime_
```

Note: If `-c` is passed to `build.py`, the `./install` directory will be deleted after the build.

A zipped version of `./install` will be created in `./archives`. The resulting filename has the format:
```
projectfarm-version-platform.zip
```

For instance:
```
projectfarm-1.0.45-darwin.zip
```

#### iOS
##### Correct as of 2021-06-12
Currently, there is not automated way of building for iOS. You can generate the iOS and iOS Simulator Xcode project files by running `generate_ios_projects.py`:

```
python3 generate_ios_projects.py
```

This will create two folders:

* `iosbuild` contains the project file for iOS
* `iossimbuild` contains the project file for iOS Simulator

You can also pass the following flags:

* `-ios` builds just the iOS project file
* `-iossim` builds just the iOS Simulator project file
* `-c` sets the build configuration. You can choose `debug` or `release`.

Please first [run `build.py`](#build) to install the assets and dependencies.

When building the iOS project, you will need to manually set the team and other signing details.

Note: This will only run on MacOS.

### CLion
##### Correct as of 2021-05-29
Open the project at the project root and CLion will recognize the base `CMakeLists.txt` file.

The supported build configurations are:

* `projectfarm_shared` This will build the shared library.
* `projectfarm_server` This will build the server and shared library.
* `projectfarm_client` This will build the client and shared library.

Set the CMake settings as follows:

> Toolchains -> Make -> /*{project_root}*/libraries/ninja/darwin/ninja

> CMake -> Toolchain -> *Set to the toolchain with Ninja as the make tool*

> CMake -> CMake options -> -GNinja -DCMAKE_CXX_COMPILER=/*{project_root}*/libraries/clang/darwin/bin/clang++ -DCMAKE_C_COMPILER=/*{project_root}*/libraries/clang/darwin/bin/clang

**Tip**: Add a `projectfarm_client` build configuration with the `-startserver` flag as a program argument. This will start the server each time the client is started, and close the server each time the client is closed.

### Visual Studio
##### Correct as of 2021-05-30
As of Visual Studio 2019 (16.9.5), LLVM crashes with an error of `out of memory`, `illegal instruction`. Due to this, building with Visual Studio is not officially supported.

## License

![GitHub](https://img.shields.io/github/license/snowmeltarcade/projectfarm?style=plastic)
