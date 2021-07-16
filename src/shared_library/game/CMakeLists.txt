target_sources(
    "${SHARED_LIBRARY_PROJECT_NAME}"
    PRIVATE
        game.cpp
    PUBLIC
        game.h
)

add_subdirectory("world")
