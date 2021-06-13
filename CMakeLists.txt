set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_minimum_required(VERSION 3.15)
project(projectfarm VERSION 1.0.46)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(INSTALL_LATEST_DIRECTORY "${CMAKE_BINARY_DIR}/install/latest/bin")
set(INSTALL_SERVER_DIRECTORY "${CMAKE_BINARY_DIR}/install/server/${PROJECT_VERSION}/bin")
set(INSTALL_CLIENT_DIRECTORY "${CMAKE_BINARY_DIR}/install/client/${PROJECT_VERSION}/bin")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake/)

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
	message(INFO "Linux == True")
	set(LINUX TRUE)
elseif(APPLE AND CMAKE_SYSTEM_NAME STREQUAL "iOS")
	message(INFO "iOS == True")
	set(IOS TRUE)
endif()

if (IOS)
	set_property(GLOBAL PROPERTY IOS_RESOURCE_FILES_FROM "")
	set_property(GLOBAL PROPERTY IOS_RESOURCE_FILES_TO "")
endif()

set("LIBRARIES_ROOT_DIR" "${CMAKE_CURRENT_LIST_DIR}/libraries")

include("${CMAKE_CURRENT_LIST_DIR}/cmake/libraries.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/Functions.cmake")

add_subdirectory("data")
add_subdirectory("src")
