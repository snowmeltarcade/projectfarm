if (MSVC)
    # Support both 32 and 64 bit builds
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
        set(LIBSODIUM_LIBRARIES "${LIBRARIES_ROOT_DIR}/libsodium/Windows/lib/libsodium.lib" CACHE INTERNAL "LIBSODIUM_LIBRARIES")
    else()
        message(ERROR "32-bit is not supported.")
    endif()

    set(LIBSODIUM_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/libsodium/windows/include" CACHE INTERNAL "LIBSODIUM_INCLUDE_DIRS")
elseif(LINUX)
    set(LIBSODIUM_LIBRARIES "${LIBRARIES_ROOT_DIR}/libsodium/Linux/lib/libsodium.a" CACHE INTERNAL "LIBSODIUM_LIBRARIES")

    set(LIBSODIUM_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/libsodium/linux/include" CACHE INTERNAL "LIBSODIUM_INCLUDE_DIRS")
else()
    set(LIBSODIUM_LIBRARIES "${LIBRARIES_ROOT_DIR}/libsodium/darwin/lib/libsodium.a" CACHE INTERNAL "LIBSODIUM_LIBRARIES")

    set(LIBSODIUM_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/libsodium/darwin/include" CACHE INTERNAL "LIBSODIUM_INCLUDE_DIRS")
endif()