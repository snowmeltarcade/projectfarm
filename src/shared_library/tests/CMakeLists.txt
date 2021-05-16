add_executable(
    "${SHARED_LIBRARY_TEST_PROJECT_NAME}"
    main.cpp
    test_util.cpp
    test_util.h
)

target_link_libraries(
    "${SHARED_LIBRARY_TEST_PROJECT_NAME}"
    Catch2::Catch2
    "${SHARED_LIBRARY_PROJECT_NAME}"
)

target_include_directories(
    "${SHARED_LIBRARY_TEST_PROJECT_NAME}"
    PRIVATE
    "${CATCH2_INCLUDE_DIRS}"
)

add_subdirectory("math")
add_subdirectory("scripts")
add_subdirectory("utils")
add_subdirectory("crypto")
add_subdirectory("graphics")
add_subdirectory("time")
add_subdirectory("markdown")
add_subdirectory("test_data")

set("TEST_DATA_DIRECTORY" "${CMAKE_CURRENT_LIST_DIR}")

catch_discover_tests("${SHARED_LIBRARY_TEST_PROJECT_NAME}")