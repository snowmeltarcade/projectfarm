target_sources(
    "${SHARED_LIBRARY_PROJECT_NAME}"
    PRIVATE
        engine.cpp
        game.cpp
        concurrency_keys.cpp
    PUBLIC
        engine.h
        game.h
        concurrency_keys.h
)

add_subdirectory("world")
