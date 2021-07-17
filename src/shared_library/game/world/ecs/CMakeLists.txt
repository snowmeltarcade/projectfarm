target_sources(
    "${SHARED_LIBRARY_PROJECT_NAME}"
    PRIVATE
        ecs.cpp
    PUBLIC
        ecs.h
)

add_subdirectory("systems")
add_subdirectory("components")
