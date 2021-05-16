target_sources(
    "${SERVER_PROJECT_NAME}"
    PUBLIC
        world.h
        island.h
        plot.h
        plots.h
        world_change_log_entry.h
        action_tile.h
    PRIVATE
        world.cpp
        island.cpp
        plot.cpp
        plots.cpp
        action_tile.cpp
)

add_subdirectory("action_tile_actions")