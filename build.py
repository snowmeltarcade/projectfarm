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

cwd = os.getcwd()
build_dir_name = "build"
install_dir_name = "install"


def configure_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--install-dependencies", action="store_true", required=False, help="install library dependencies before build")
    parser.add_argument("-nb", "--no-build", action="store_true", required=False, help="do not perform build")
    parser.add_argument("-ni", "--no-install", action="store_true", required=False, help="do not install after build")
    parser.add_argument("-c", "--cleanup", action="store_true", required=False, help="clean up build and install data/temp files after build")
    args = parser.parse_args()

    return args


def install_dependencies():
    print("Installing dependencies...")

    temp_dir = os.path.join(cwd, "__temp")
    make_dir(temp_dir)

    os.chdir(temp_dir)

    print("Clonging project dependencies...")

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

def do_make(no_build):
    print("Making...")

    build_dir = os.path.join(cwd, build_dir_name)
    make_dir(build_dir)

    os.chdir(build_dir)

    platform_name = get_platform_name()

    clang_directory = os.path.join(cwd, "libraries", "clang-12", f"{platform_name}", "bin", "clang")
    clangxx_directory = os.path.join(cwd, "libraries", "clang-12", f"{platform_name}", "bin", "clang++")

    if not no_build:
        #run_cmd([cmake_path, "-GNinja", f"-DCMAKE_C_COMPILER={clang_directory}", f"-DCMAKE_CXX_COMPILER={clangxx_directory}", ".."])
        run_cmd([cmake_path, ".."])
        run_cmd([cmake_path, "--build", ".", "--config", "Release", "--verbose"])
        #run_cmd([ctest_path])
        run_cmd([cmake_path, "--install", ".", "--config", "Release"])

    install_src_dir = os.path.join(build_dir, install_dir_name)
    install_dest_dir = os.path.join(cwd, install_dir_name)

    make_dir(install_dest_dir)

    copy_dir(install_src_dir, install_dest_dir)

    print("Made.")

    return (build_dir, install_dest_dir)


def do_install(install_dir):
    print("Installing...")

    format = "zip"
    version = get_project_version()

    archive_name = os.path.join(cwd, "archives", f"projectfarm-{version}-{platform.system().lower()}")

    shutil.make_archive(archive_name, format, install_dir)

    print(f"Created archive: {archive_name}.{format}")

    print("Installed.")


def run_cmd(cmd):
    print(f"Running command: {cmd}")
    subprocess.run(cmd)


def make_dir(dir):
    try:
        os.mkdir(dir, 0o755)
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

if args.install_dependencies:
    install_dependencies()

build_dir, install_dir = do_make(args.no_build)

if not args.no_install:
    do_install(install_dir)

if args.cleanup:
    cleanup_environment(build_dir, install_dir)

print("Finished build.")
