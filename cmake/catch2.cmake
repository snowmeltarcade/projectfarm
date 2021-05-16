add_subdirectory("${LIBRARIES_ROOT_DIR}/catch2")

find_package(Catch2 REQUIRED)

enable_testing()

include(CTest)
include("${LIBRARIES_ROOT_DIR}/catch2/contrib/Catch.cmake")