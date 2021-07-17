target_sources(
    "${SHARED_LIBRARY_PROJECT_NAME}"
    PRIVATE
        ecs.cpp
    PUBLIC
        ecs.h
        iecs_component.h
)

add_subdirectory("systems")
