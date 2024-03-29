if (WIN32)
	# as we use clang on Windows, make sure this is properly defined
	# SDL needs __WIN32__
	add_definitions(-DWIN32 -D__WIN32__)

	# SDL defines its own main, so ensure we choose the correct main
	add_link_options(-Wl,-subsystem:console)
elseif (IOS)
	# for SDL
	add_definitions(-D__IPHONEOS__)
endif()

if (MSVC)
	# although the builder will not use MSVC to build, it might be used for dev at times
	if (IS_DEBUG)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
	else()
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
	endif()

	# max warnings; warnings as errors; ignore warnings from system headers; compile using standard C++ unwind semantics
	add_compile_options(/W4 /WX /external:anglebrackets /external:W0 /experimental:external /EHsc /MT)
else()
	if (IS_DEBUG)
		# -fsanitize=memory is not supported on MacOS
		# -fsanitize=thread is not allowed with =address
		# -fno-sanitize=vptr is needed to link against v8
		#set(ASAN_FLAGS "-fsanitize=address,undefined -fno-omit-frame-pointer")
        # valgrind requires no address sanitizer
		set(ASAN_FLAGS "")

		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 ${ASAN_FLAGS} -fno-sanitize=vptr")
		#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
		#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${ASAN_FLAGS}")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${ASAN_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
	endif()

	add_compile_options(-Wall -Wextra -pedantic -Werror -pedantic-errors -Wshadow -Wstrict-aliasing)

	if (LINUX)
		set(CMAKE_CXX_FLAGS "-pthread")
	endif()
endif()

configure_file("version.h.in" "version.h")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory("shared_library")
add_subdirectory("server_main")
add_subdirectory("application_main")

if (NOT LINUX)
	add_subdirectory("client_main")
endif()