add_executable(
    "${SHARED_LIBRARY_TEST_PROJECT_NAME}"
    main.cpp
    test_util.cpp
    test_util.h
)

if (IOS)
	set_target_properties(
		"${SHARED_LIBRARY_TEST_PROJECT_NAME}"
		PROPERTIES
			MACOSX_BUNDLE TRUE
			MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
	)
endif()

target_link_libraries(
    "${SHARED_LIBRARY_TEST_PROJECT_NAME}"
    Catch2::Catch2
    "${SHARED_LIBRARY_PROJECT_NAME}"
)

target_include_directories(
    "${SHARED_LIBRARY_TEST_PROJECT_NAME}"
    SYSTEM PRIVATE
    "${CATCH2_INCLUDE_DIRS}"
)

target_include_directories(
	"${SHARED_LIBRARY_TEST_PROJECT_NAME}"
	SYSTEM PRIVATE
	${SDL2_INCLUDE_DIRS}
)

target_link_libraries(
	"${SHARED_LIBRARY_TEST_PROJECT_NAME}"
	${SDL2_LIBRARIES}
)

# to link against v8
if (WIN32)
	set_property(TARGET "${SHARED_LIBRARY_TEST_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

add_subdirectory("api")
add_subdirectory("math")
add_subdirectory("scripts")
add_subdirectory("utils")
add_subdirectory("crypto")
add_subdirectory("graphics")
add_subdirectory("time")
add_subdirectory("markdown")
add_subdirectory("css")
add_subdirectory("test_data")
add_subdirectory("concurrency")

set("TEST_DATA_DIRECTORY" "${CMAKE_CURRENT_LIST_DIR}")

catch_discover_tests("${SHARED_LIBRARY_TEST_PROJECT_NAME}")