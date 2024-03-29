add_executable(
    "${SERVER_PROJECT_NAME}"
    main.cpp
)

if (IOS)
	SetupiOSBuild("${SERVER_PROJECT_NAME}" "server" "Server")
endif()

install(
	TARGETS
		"${SERVER_PROJECT_NAME}"
	DESTINATION
		"${CMAKE_BINARY_DIR}/install/server/${PROJECT_VERSION}/bin"
)

install(
	TARGETS
		"${SERVER_PROJECT_NAME}"
	DESTINATION
		"${CMAKE_BINARY_DIR}/install/latest/bin"
)

target_link_libraries(
    "${SERVER_PROJECT_NAME}"
    PRIVATE
    "${SHARED_LIBRARY_PROJECT_NAME}"
	"${SDL2_LIBRARIES}"
	"${SDL2_NET_LIBRARIES}"
)

target_include_directories(
	"${SERVER_PROJECT_NAME}"
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(
	"${SERVER_PROJECT_NAME}"
	SYSTEM PRIVATE
	${SDL2_INCLUDE_DIRS}
)

target_include_directories(
    "${SERVER_PROJECT_NAME}"
    SYSTEM PRIVATE
    ${SDL2_NET_INCLUDE_DIRS}
)

if(IS_DEBUG)
	target_compile_definitions(
		"${SERVER_PROJECT_NAME}"
		PRIVATE
		LOGGING_PACKET_DEBUG_INFO
	)
endif()

# to link against v8
if (WIN32)
	set_property(TARGET "${SERVER_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

add_subdirectory("engine")
add_subdirectory("server")