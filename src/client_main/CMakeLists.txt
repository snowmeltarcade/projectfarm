add_executable(
	"${CLIENT_PROJECT_NAME}"
	main.cpp
)

if (IOS)
	SetupiOSBuild("${CLIENT_PROJECT_NAME}" "client" "Client")
endif()

install(
	TARGETS
		"${CLIENT_PROJECT_NAME}"
	DESTINATION
		"${CMAKE_BINARY_DIR}/install/client/${PROJECT_VERSION}/bin"
)

install(
	TARGETS
		"${CLIENT_PROJECT_NAME}"
	DESTINATION
		"${CMAKE_BINARY_DIR}/install/latest/bin"
)

target_include_directories(
	"${CLIENT_PROJECT_NAME}"
	PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(
	"${CLIENT_PROJECT_NAME}"
	SYSTEM PRIVATE
	${SDL2_INCLUDE_DIRS}
)

target_link_libraries(
	"${CLIENT_PROJECT_NAME}"
	${SDL2_LIBRARIES}
)

target_include_directories(
	"${CLIENT_PROJECT_NAME}"
	SYSTEM PRIVATE
	${SDL2_NET_INCLUDE_DIRS}
)

target_link_libraries(
	"${CLIENT_PROJECT_NAME}"
	${SDL2_NET_LIBRARIES}
)

target_include_directories(
	"${CLIENT_PROJECT_NAME}"
	SYSTEM PRIVATE
	${SDL2_IMG_INCLUDE_DIRS}
)

target_link_libraries(
	"${CLIENT_PROJECT_NAME}"
	${SDL2_IMG_LIBRARIES}
)

target_link_libraries(
    "${CLIENT_PROJECT_NAME}"
    "${SHARED_LIBRARY_PROJECT_NAME}"
)

target_link_libraries(
	"${CLIENT_PROJECT_NAME}"
	"nlohmann_json::nlohmann_json"
)

if (IOS)
	find_package(OpenGLES REQUIRED)
	target_link_libraries(
		"${CLIENT_PROJECT_NAME}"
		"${OPENGLES_LIBRARIES}"
	)

	target_include_directories(
		"${CLIENT_PROJECT_NAME}"
		SYSTEM PRIVATE
		${OPENGLES_INCLUDE_DIR}
	)
else()
	find_package(OpenGL REQUIRED)
	target_link_libraries(
		"${CLIENT_PROJECT_NAME}"
		OpenGL::GL
	)
endif()

target_include_directories(
	"${CLIENT_PROJECT_NAME}"
	SYSTEM PRIVATE
	${GLEW_INCLUDE_DIRS}
)

target_link_libraries(
	"${CLIENT_PROJECT_NAME}"
	"${GLEW_LIBRARIES}"
)

if(IS_DEBUG)
	target_compile_definitions(
		"${CLIENT_PROJECT_NAME}"
		PRIVATE
		LOGGING_PACKET_DEBUG_INFO
	)
endif()

if (WIN32)
	file(
		COPY "${GLEW_DLL_PATH}"
		DESTINATION "${PROJECT_BINARY_DIR}/bin"
	)
endif()

# to link against v8
if (WIN32)
	set_property(TARGET "${CLIENT_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

add_subdirectory("engine")
add_subdirectory("scenes")
add_subdirectory("graphics")
add_subdirectory("network_client")