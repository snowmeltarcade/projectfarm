target_sources(
	"${CLIENT_PROJECT_NAME}"
	PRIVATE
		scene_manager.cpp
		scene.cpp
	PUBLIC
		scene_types.h
		scene.h
		scene_manager.h
		consume_scene_manager.h
		consume_scene.h
)

add_subdirectory("implemented_scenes")