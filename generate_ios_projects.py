import os
import re
import shutil
import platform
import subprocess
import argparse

cmake_path = shutil.which("cmake")

ios_dir_name = "iosbuild"
ios_simulator_dir_name = "iossimbuild"


def configure_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("-ios", "--ios", action="store_true", required=False, help="generate only iOS project")
    parser.add_argument("-iossim", "--ios-simulator", action="store_true", required=False, help="generate only iOS Simulator project")
    args = parser.parse_args()

    return args


def run_cmd(cmd):
    print(f"Running command: {cmd}")
    subprocess.run(cmd)


def make_dir(dir):
    try:
        os.makedirs(dir, 0o755, exist_ok=True)
    except OSError:
        print(f"Failed to create dir: {dir}")
    else:
        print(f"Created dir: {dir}")


def remove_dir(dir):
    try:
        shutil.rmtree(dir)
    except OSError:
        print(f"Failed to remove dir: {dir}")
    else:
        print(f"Removed dir: {dir}")


def generate_projects(ios, ios_sim):

    # if no flags were passed, generate both iOS and iOS Simulator projects
    if ios:
        generate_ios_project()
    elif ios_sim:
        generate_ios_simulator_project()
    else:
        generate_ios_project()
        generate_ios_simulator_project()


def generate_ios_project():
    print("Generating iOS project...")

    cwd = os.getcwd()

    path = os.path.join(cwd, ios_dir_name)

    remove_dir(path)
    make_dir(path)

    os.chdir(path)

    cmd = [cmake_path, "..", "-GXcode", "-DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake", "-DPLATFORM=OS64"]
    run_cmd(cmd)

    os.chdir(cwd)

    print("Generated iOS project.")


def generate_ios_simulator_project():
    print("Generating iOS Simulator project...")

    cwd = os.getcwd()

    path = os.path.join(cwd, ios_simulator_dir_name)

    remove_dir(path)
    make_dir(path)

    os.chdir(path)

    cmd = [cmake_path, "..", "-GXcode", "-DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake", "-DPLATFORM=SIMULATOR64"]
    run_cmd(cmd)

    os.chdir(cwd)

    print("Generated iOS Simulator project.")


print("Generating projects...")

if platform.system() != "Darwin":
    print("Can only generate iOS projects on MacOS.")
else:
    args = configure_arguments()

    generate_projects(args.ios, args.ios_simulator)

print("Generated projects.")
