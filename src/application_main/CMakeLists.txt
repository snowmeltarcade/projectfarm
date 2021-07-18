add_executable(
	"${APPLICATION_PROJECT_NAME}"
	main.cpp
)

if (IOS)
	SetupiOSBuild("${APPLICATION_PROJECT_NAME}" "application" "Application")
endif()

install(
	TARGETS
		"${APPLICATION_PROJECT_NAME}"
	DESTINATION
		"${CMAKE_BINARY_DIR}/install/application/${PROJECT_VERSION}/bin"
)

install(
	TARGETS
		"${APPLICATION_PROJECT_NAME}"
	DESTINATION
		"${CMAKE_BINARY_DIR}/install/latest/bin"
)

target_include_directories(
	"${APPLICATION_PROJECT_NAME}"
	PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(
	"${APPLICATION_PROJECT_NAME}"
	SYSTEM PRIVATE
	${SDL2_INCLUDE_DIRS}
)

target_link_libraries(
	"${APPLICATION_PROJECT_NAME}"
	${SDL2_LIBRARIES}
)

target_link_libraries(
    "${APPLICATION_PROJECT_NAME}"
    "${SHARED_LIBRARY_PROJECT_NAME}"
)

if (IOS)
	find_package(OpenGLES REQUIRED)
	target_link_libraries(
		"${APPLICATION_PROJECT_NAME}"
		"${OPENGLES_LIBRARIES}"
	)

	target_include_directories(
		"${APPLICATION_PROJECT_NAME}"
		SYSTEM PRIVATE
		${OPENGLES_INCLUDE_DIR}
	)
else()
	find_package(OpenGL REQUIRED)
	target_link_libraries(
		"${APPLICATION_PROJECT_NAME}"
		OpenGL::GL
	)
endif()

target_include_directories(
	"${APPLICATION_PROJECT_NAME}"
	SYSTEM PRIVATE
	${GLEW_INCLUDE_DIRS}
)

target_link_libraries(
	"${APPLICATION_PROJECT_NAME}"
	"${GLEW_LIBRARIES}"
)

if (WIN32)
	file(
		COPY "${GLEW_DLL_PATH}"
		DESTINATION "${PROJECT_BINARY_DIR}/bin"
	)
endif()

# to link against v8
if (WIN32)
	set_property(TARGET "${APPLICATION_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()
