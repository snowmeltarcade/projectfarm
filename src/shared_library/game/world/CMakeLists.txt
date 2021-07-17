target_sources(
    "${SHARED_LIBRARY_PROJECT_NAME}"
    PRIVATE
        world.cpp
    PUBLIC
        world.h
)

add_subdirectory("ecs")
add_subdirectory("controllers")
