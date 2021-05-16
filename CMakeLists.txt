set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (LINUX)
	set(CMAKE_CXX_FLAGS "-pthread")
endif()

cmake_minimum_required(VERSION 3.15)
project(projectfarm VERSION 1.0.45)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(INSTALL_LATEST_DIRECTORY "${CMAKE_BINARY_DIR}/install/latest/bin")
set(INSTALL_SERVER_DIRECTORY "${CMAKE_BINARY_DIR}/install/server/${PROJECT_VERSION}/bin")
set(INSTALL_CLIENT_DIRECTORY "${CMAKE_BINARY_DIR}/install/client/${PROJECT_VERSION}/bin")

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

message("Build mode: ${CMAKE_BUILD_TYPE}")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set(IS_DEBUG true)
endif()

set(SHARED_LIBRARY_PROJECT_NAME "${PROJECT_NAME}_shared")
set(SHARED_LIBRARY_TEST_PROJECT_NAME "${PROJECT_NAME}_shared_test")
set(SERVER_PROJECT_NAME "${PROJECT_NAME}_server")
set(CLIENT_PROJECT_NAME "${PROJECT_NAME}_client")

if(IS_DEBUG)
	add_compile_definitions(DEBUG)
else()
	add_compile_definitions(RELEASE)
endif()

if (UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

if (MSVC)
	if (IS_DEBUG)
		set(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
	endif()

	# the final forward slash is needed to ensure that we copy the files within the folder, not the actual
	# folder itself
	set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
	set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/system_runtimes/")
	include(InstallRequiredSystemLibraries)
endif()

install(
	FILES
		${CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION}
	DESTINATION
		${INSTALL_CLIENT_DIRECTORY}
)

install(
	FILES
		${CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION}
	DESTINATION
		${INSTALL_SERVER_DIRECTORY}
)

install(
	FILES
		${CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION}
	DESTINATION
		${INSTALL_LATEST_DIRECTORY}
)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/Functions.cmake")

set("LIBRARIES_ROOT_DIR" "${CMAKE_CURRENT_LIST_DIR}/libraries/")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/libraries.cmake")

#find_package(Catch2 REQUIRED)

#enable_testing()

#include(CTest)
#include(Catch)

#add_subdirectory("src")
#add_subdirectory("data")