target_sources(
    "${SHARED_LIBRARY_PROJECT_NAME}"
    PRIVATE
        system.cpp
    PUBLIC
        system.h
)

add_subdirectory("windows")
