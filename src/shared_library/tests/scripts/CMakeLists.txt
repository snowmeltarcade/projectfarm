target_sources(
    "${SHARED_LIBRARY_TEST_PROJECT_NAME}"
    PRIVATE
        script.cpp
        script_system.cpp
)

add_subdirectory("scripts")