import os
import re
import shutil
import platform
import subprocess
import zipfile

git_path = shutil.which("git")
cmake_path = shutil.which("cmake")
python3_path = shutil.which("python3")

cwd = os.getcwd()
build_dir_name = "build"
install_dir_name = "install"


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


def do_make():
    print("Making...")

    build_dir = os.path.join(cwd, "build")
    make_dir(build_dir)

    os.chdir(build_dir)

    run_cmd([cmake_path, ".."])
    run_cmd([cmake_path, "--build", ".", "--config", "Release"])
    #run_cmd([cmake_path, "--install", ".", "--config", "Release"])
    #run_cmd([cmake_path, "--verbose"])

    install_src_dir = os.path.join(build_dir, install_dir_name)
    install_dest_dir = os.path.join(cwd, install_dir_name)

    copy_dir(install_src_dir, install_dest_dir)

    print("Made.")

    return install_dest_dir


def do_install(install_dir):
    print("Installing...")

    format = "zip"
    version = get_project_version()

    archive_name = os.path.join(cwd, f"projectfarm-{version}-{platform.system().lower()}")

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
        shutil.copytree(src, dest)
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

#install_dependencies()

install_dir = do_make()
#do_install(install_dir)
#cleanup_environment(build_dir, install_dir)

print("Finished build.")
