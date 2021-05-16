target_sources(
    "${SERVER_PROJECT_NAME}"
    PUBLIC
        player.h
        player_load_details.h
    PRIVATE
        player.cpp
        player_load_details.cpp
)

add_subdirectory("data")
add_subdirectory("world")
add_subdirectory("entities")
add_subdirectory("scripting")