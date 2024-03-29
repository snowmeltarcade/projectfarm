add_library(
    "${SHARED_LIBRARY_PROJECT_NAME}"
)

if (IOS)
	set_target_properties(
		"${SHARED_LIBRARY_PROJECT_NAME}"
		PROPERTIES
			MACOSX_BUNDLE TRUE
			MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
			XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.snowmeltarcade.projectfarm.shared"
			XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.snowmeltarcade.projectfarm.shared"
	)

	configure_file("Info.plist.in" "Info.plist")
endif()

target_include_directories(
    "${SHARED_LIBRARY_PROJECT_NAME}"
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(
	"${SHARED_LIBRARY_PROJECT_NAME}"
	SYSTEM PRIVATE
	${SDL2_INCLUDE_DIRS}
)

target_link_libraries(
	"${SHARED_LIBRARY_PROJECT_NAME}"
	${SDL2_LIBRARIES}
)

target_include_directories(
    "${SHARED_LIBRARY_PROJECT_NAME}"
    SYSTEM PRIVATE
    ${SDL2_NET_INCLUDE_DIRS}
)

target_link_libraries(
    "${SHARED_LIBRARY_PROJECT_NAME}"
    ${SDL2_NET_LIBRARIES}
)

target_link_libraries(
	"${SHARED_LIBRARY_PROJECT_NAME}"
	"nlohmann_json::nlohmann_json"
)

target_link_libraries(
	"${SHARED_LIBRARY_PROJECT_NAME}"
	"${SQLITE_PROJECT_NAME}::${SQLITE_PROJECT_NAME}"
)

# to link against v8
if (WIN32)
	set_property(TARGET "${SHARED_LIBRARY_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

target_include_directories(
	"${SHARED_LIBRARY_PROJECT_NAME}"
	SYSTEM PUBLIC
	"${V8_INCLUDE_DIRS}"
)

target_link_libraries(
	"${SHARED_LIBRARY_PROJECT_NAME}"
	${V8_LIBRARIES}
)

target_include_directories(
	"${SHARED_LIBRARY_PROJECT_NAME}"
	SYSTEM PUBLIC
	"${LIBSODIUM_INCLUDE_DIRS}"
)

target_link_libraries(
	"${SHARED_LIBRARY_PROJECT_NAME}"
	${LIBSODIUM_LIBRARIES}
)

# Used for libsodium
add_definitions(-DSODIUM_STATIC)

if (LINUX)
	target_link_libraries(
		"${SHARED_LIBRARY_PROJECT_NAME}"
		-ldl
	)
endif()

if (WIN32)
	file (
		COPY "${SDL2_DLL_PATH}"
		DESTINATION "${PROJECT_BINARY_DIR}/bin"
	)

	file (
		COPY "${SDL2_NET_DLL_PATH}"
		DESTINATION "${PROJECT_BINARY_DIR}/bin"
	)

	foreach(DLL_PATH ${SDL2_IMG_DLL_PATH})
		file (
			COPY "${DLL_PATH}"
			DESTINATION "${PROJECT_BINARY_DIR}/bin"
		)
	endforeach()
endif()

add_subdirectory("api")
add_subdirectory("utils")
add_subdirectory("networking")
add_subdirectory("data")
add_subdirectory("entities")
add_subdirectory("state")
add_subdirectory("math")
add_subdirectory("tests")
add_subdirectory("scripting")
add_subdirectory("persistence")
add_subdirectory("crypto")
add_subdirectory("time")
add_subdirectory("graphics")
add_subdirectory("markdown")
add_subdirectory("platform")
add_subdirectory("css")
add_subdirectory("game")
add_subdirectory("concurrency")
