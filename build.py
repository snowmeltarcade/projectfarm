import os
import re
import shutil
import platform
import subprocess
import argparse

git_path = shutil.which("git")
cmake_path = shutil.which("cmake")
ctest_path = shutil.which("ctest")
python3_path = shutil.which("python3")
ninja_path = shutil.which("ninja")

cwd = os.getcwd()
build_dir_name = "build"
install_dir_name = "install"


def configure_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--install-assets", action="store_true", required=False, help="install assets before build")
    parser.add_argument("-i", "--install-dependencies", action="store_true", required=False, help="install library dependencies before build")
    parser.add_argument("-nb", "--no-build", action="store_true", required=False, help="do not perform build")
    parser.add_argument("-ni", "--no-install", action="store_true", required=False, help="do not install after build")
    parser.add_argument("-an", "--archive-name", action="store", required=False, help="what to name the built archive")
    parser.add_argument("-c", "--cleanup", action="store_true", required=False, help="clean up build and install data/temp files after build")
    parser.add_argument("-ios", "--ios", action="store_true", required=False, help="build for iOS. Note: Only works on MacOS")
    parser.add_argument("-iossim", "--ios-simulator", action="store_true", required=False, help="build for iOS Simulator. Note: Only works on MacOS")
    args = parser.parse_args()

    return args


def install_assets():
    print("Installing assets...")

    temp_dir = os.path.join(cwd, "__temp")
    make_dir(temp_dir)

    os.chdir(temp_dir)

    print("Cloning assets...")

    cmd = [git_path, "clone", "https://github.com/snowmeltarcade/projectfarm-assets.git"]
    run_cmd(cmd)

    print("Installing project assets...")

    project_assets_path = os.path.join(temp_dir, "projectfarm-assets")
    os.chdir(project_assets_path)

    # cwd = project path
    cmd = [python3_path, "install.py", "-p", cwd]
    run_cmd(cmd)

    os.chdir(cwd)

    remove_dir(temp_dir)

    print("Installed assets.")


def install_dependencies():
    print("Installing dependencies...")

    temp_dir = os.path.join(cwd, "__temp")
    make_dir(temp_dir)

    os.chdir(temp_dir)

    print("Cloning project dependencies...")

    cmd = [git_path, "clone", "https://github.com/snowmeltarcade/project-dependencies.git"]
    run_cmd(cmd)

    print("Installing project dependencies...")
    
    project_dependencies_path = os.path.join(temp_dir, "project-dependencies")
    os.chdir(project_dependencies_path)

    # cwd = project path
    cmd = [python3_path, "install_all.py", "-p", cwd]
    run_cmd(cmd)

    os.chdir(cwd)

    remove_dir(temp_dir)

    print("Installed dependencies.")


def cleanup_environment(build_dir, install_dir):
    print("Cleaning up build environment...")

    os.chdir(cwd)

    remove_dir(build_dir)
    remove_dir(install_dir)

    print("Build environment prepared.")


def get_platform_name():
    if platform.system() == "Darwin":
        return "darwin"
    elif platform.system() == "Linux":
        return "linux"
    else:
        return "windows"

def do_make(no_build, ios, ios_simulator):
    print("Making...")

    if (ios or ios_simulator) and platform.system() != "Darwin":
        print("Can only build iOS and iOS Simulator on MacOS")
        return

    build_dir = os.path.join(cwd, build_dir_name)
    make_dir(build_dir)

    os.chdir(build_dir)

    platform_name = get_platform_name()

    clang_path = os.path.join(cwd, "libraries", "clang", platform_name, "bin")

    clang_directory = os.path.join(clang_path, "clang")
    clangxx_directory = os.path.join(clang_path, "clang++")
    llvmrc_directory = os.path.join(clang_path, "llvm-rc.exe")

    if platform.system() == "Windows":
        # we get cmake errors if we use backslash on Windows, so
        # ensure we always use a forward slash
        clang_directory = clang_directory.replace("\\", "/") + ".exe"
        clangxx_directory = clangxx_directory.replace("\\", "/") + ".exe"
        llvmrc_directory = llvmrc_directory.replace("\\", "/") + ".exe"

    if not no_build:
        cmd = [cmake_path]

        # don't use Ninja when building for iOS/iOS Simulator
        if ios or ios_simulator:
            cmd += ["-GXcode",
                    "-DCMAKE_SYSTEM_NAME=iOS",
                    "-DCMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH=NO",
                    "-DCMAKE_OSX_DEPLOYMENT_TARGET=9.3",
                    "-DMACOSX_BUNDLE_BUNDLE_NAME=ProjectFarm",
                    f"-DMACOSX_BUNDLE_BUNDLE_VERSION={get_project_version()}",
                    f"-MACOSX_BUNDLE_LONG_VERSION_STRING={get_project_version()}",
                    f"-MACOSX_BUNDLE_SHORT_VERSION_STRING={get_project_version()}",
                    "-DMACOSX_BUNDLE_COPYRIGHT=\"Snow Melt Arcade © 2021\"",
                    "-DMACOSX_BUNDLE_INFO_STRING=© 2021",
                    "-DMACOSX_BUNDLE_GUI_IDENTIFIER=com.snowmeltarcade.projectfarm"]
                    
            if ios:
                cmd += ["-DCMAKE_OSX_ARCHITECTURES=arm64"]
            else:
                cmd += ["-DCMAKE_OSX_ARCHITECTURES=x86_64"]
        else:
            ninja_path = os.path.join(cwd, "libraries", "ninja", platform_name, "ninja")
            if platform.system() == "Windows":
                ninja_path = ninja_path.replace("\\", "/") + ".exe"

            cmd += [f"-DCMAKE_MAKE_PROGRAM={ninja_path}", "-GNinja"]

        cmd += [f"-DCMAKE_C_COMPILER={clang_directory}",
                f"-DCMAKE_CXX_COMPILER={clangxx_directory}",
                f"-DCMAKE_RC_COMPILER={llvmrc_directory}",
                ".."]

        run_cmd(cmd)

        cmd = [cmake_path, "--build", ".", "--config", "Release", "--verbose"]

        if ios:
            cmd += ["--", "-sdk", "iphoneos"]
        elif ios_simulator:
            cmd += ["--", "-sdk", "iphonesimulator"]

        run_cmd(cmd)

        cmd = [ctest_path]
        run_cmd(cmd)

        cmd = [cmake_path, "--install", ".", "--config", "Release"]
        run_cmd(cmd)

    install_src_dir = os.path.join(build_dir, install_dir_name)
    install_dest_dir = os.path.join(cwd, install_dir_name, platform.system())

    make_dir(install_dest_dir)

    copy_dir(install_src_dir, install_dest_dir)

    print("Made.")

    return (build_dir, install_dest_dir)


def do_install(install_dir, archive_name_override):
    print("Installing...")

    format = "zip"
    version = get_project_version()

    archive_file_name = f"projectfarm-{version}-{platform.system().lower()}"

    if not archive_name_override is None and len(archive_name_override) > 0:
        archive_file_name = archive_name_override

    archive_name = os.path.join(cwd, "archives", archive_file_name)

    shutil.make_archive(archive_name, format, install_dir)

    print(f"Created archive: {archive_name}.{format}")

    print("Installed.")


def run_cmd(cmd):
    print(f"Running command: {cmd}")
    subprocess.run(cmd)


def run_cmd_env(cmd, env):
    print(f"Running command: {cmd}")
    subprocess.run(cmd, env=env)


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


def copy_dir(src, dest):
    try:
        shutil.copytree(src, dest, dirs_exist_ok=True)
    except OSError:
        print(f"Failed to copy: {src} to: {dest}")
    else:
        print(f"Copied: {src} to: {dest}")


def get_project_version():
    makefile_dir = os.path.join(cwd, "CMakeLists.txt")

    file_contents = ""

    with open(makefile_dir) as f:
        file_contents = f.read()

    match = re.search(r'projectfarm VERSION (\d+\.\d+\.\d+)', file_contents)

    version = match.group(1)

    return version


print("Starting build...")

args = configure_arguments()

if args.install_assets:
    install_assets()

if args.install_dependencies:
    install_dependencies()

build_dir, install_dir = do_make(args.no_build, args.ios, args.ios_simulator)

if not args.no_install:
    do_install(install_dir, args.archive_name)

if args.cleanup:
    cleanup_environment(build_dir, install_dir)

print("Finished build.")
