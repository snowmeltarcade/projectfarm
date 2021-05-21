# Project Farm
A multiplayer farming simulator.


## Build
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