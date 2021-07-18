target_sources(
    "${SHARED_LIBRARY_PROJECT_NAME}"
    PRIVATE
        game.cpp
        concurrency_keys.cpp
    PUBLIC
        game.h
        concurrency_keys.h
)

add_subdirectory("world")
