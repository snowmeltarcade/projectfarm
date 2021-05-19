# Project Farm
A multiplayer farming simulator.


## Build
##### Correct as of 2021-05-19

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