include("${CMAKE_CURRENT_LIST_DIR}/sdl.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/v8.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/nlohmann_json.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/libsodium.cmake")

add_subdirectory("${LIBRARIES_ROOT_DIR}/catch2")
add_subdirectory("${LIBRARIES_ROOT_DIR}/sqlite3")